#include <iostream>
#include <deque>
#include <boost/optional.hpp>

#include "phase1/tokenize.h"
#include "phase2/parse.h"
#include "phase3/resolve_and_verify.h"
#include "phase5/C99/to_C99.h"
#include "phase5/callable/to_callable.h"
#include "Template.h"


namespace quatsch { namespace extern_template {

class Test : public Template
{
public:
        Test() : Template("Test",
                          StaticArgumentMeta("foo", StaticType::String),
                          StaticArgumentMeta("bar", StaticType::Float),
                          StaticArgumentMeta("frob", StaticType::Float, required))
        {}

private:
        Instantiation instantiate_ (std::list<StaticParameter> const &) const;
};

Instantiation
Test::instantiate_ (std::list<StaticParameter> const &params) const
{
        for (auto const &p : params) {
                std::cout << p.name() << " : ";
                switch (p.type()) {
                case Integer: std::cout << "int <- " << p.integer(); break;
                case Float:   std::cout << "float <- " << p.floating(); break;
                case String:  std::cout << "string <- " << p.string(); break;
                }
                std::cout << '\n';
        }
        return { [](DynamicArguments const &args){
                        return DynamicVariant::Floating(args[0].floating()*
                                                        args[1].floating());

                 },
                 Typename::Float,
                 {Typename::Float, Typename::Float}
               };
}

} }


int main () {
        using namespace quatsch::extern_template;
        std::cout << '\n';

        using namespace quatsch::compiler;
        const std::string code =
        "\n"
        "(let pi:float 3.14159)\n"
        "(let two:float 3.14159)\n"
        "(defun main:int (x:int y) (main 2i 3))\n" // TODO: shall we? would have re-nameable program input
        //"([Test frob{3} foo{1 2 3}] 2 2)"
        "(defun T (x:int) (/ y y))\n"
        "([Test frob{1.5}] x y)"

        ;
        std::cout << "quatsch code:\n------------\n" << code
                  << "\n------------\n";

        std::cout << "-- phase 1 -----" << std::endl;
        phase1::Toque toks = phase1::tokenize (code.begin(), code.end());

        std::cout << "-- phase 2 -----" << std::endl;
        phase2::ProgramPtr P2 = phase2::parse(toks);
        if (!P2) {
                std::cerr << "parse error" << std::endl;
                return 0;
        }

        std::cout << "-- phase 3 -----" << std::endl;
        std::list<quatsch::extern_template::TemplatePtr> templates;
        templates.emplace_back (new Test());
        phase3::ProgramPtr P3 = phase3::resolve_and_verify (*P2, templates);
        if (!P3) {
                std::cerr << "verficiation/resolution error" << std::endl;
                return 0;
        }
        std::string c99 = phase5::to_C99 (*P3);
        std::cout << "C99 code:\n------------\n" << c99
                  << "\n------------\n";

        auto fun = phase5::to_callable (*P3);
        DynamicArguments args ({DynamicVariant::Floating(0.5),
                                DynamicVariant::Floating(0.5)});
        std::cout << fun (args).floating() << std::endl;
}


