//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2010  Sebastian Mach (*1983)
// * mail: phresnel/at/gmail/dot/com
// * http://phresnel.org
// * http://picogen.org
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef QUATSCH_HEIGHT_FUNCTION_HH_INCLUDED_20100227
#define QUATSCH_HEIGHT_FUNCTION_HH_INCLUDED_20100227

#include "height-function.hh"
#include "distribution-function.hh"

#include "quatsch/quatsch.hh"
#include "quatsch/frontend/jux.hh"
#include "quatsch/backend/est/backend.hh"

#include "quatsch/configurable-functions/noise2ddef.hh"
#include "quatsch/configurable-functions/layerednoise2ddef.hh"
#include "quatsch/configurable-functions/heightmapdef.hh"
#include "quatsch/configurable-functions/libnoise-support-def.hh"

namespace picogen { namespace redshift {
template <typename BASE>
class quatsch_function : public BASE {
private:
        // quatsch
        typedef quatsch::backend::est::Backend <redshift::real_t, const redshift::real_t *> backend_t;
        typedef backend_t::Function Function;
        typedef backend_t::FunctionPtr FunctionPtr;
        typedef backend_t::scalar_t scalar_t;
        typedef backend_t::parameters_t parameters_t;
        typedef quatsch::frontend::jux::Compiler <backend_t> Compiler;
        typedef Compiler::ConfigurableFunctionsMap FunctionSet;

        static Compiler::ConfigurableFunctionsMap addfuns() {
                using namespace redshift;


                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr noiseDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: Noise2d <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr layeredNoise2dDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LayeredNoise2d <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr heightmapDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: Heightmap <Function, Compiler>,
                                Function
                        >
                );

                // libnoise
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr libnoisePerlinDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LibnoisePerlin <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr libnoiseBillowDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LibnoiseBillow <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr libnoiseCylindersDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LibnoiseCylinders <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr libnoiseRidgedMultiDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LibnoiseRidgedMulti <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr libnoiseSpheresDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LibnoiseSpheres <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr libnoiseVoronoiDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LibnoiseVoronoi <Function, Compiler>,
                                Function
                        >
                );
                quatsch::ICreateConfigurableFunction<Function>::ConfigurableFunctionDescriptionPtr libnoiseTurbulenceDesc (
                        new quatsch::CreateConfigurableFunction <
                                quatsch :: configurable_functions :: LibnoiseTurbulence <Function, Compiler>,
                                Function
                        >
                );

                Compiler::ConfigurableFunctionsMap addfuns;
                addfuns.addSymbol ("Noise2d", noiseDesc);
                addfuns.addSymbol ("ValueNoise2d", layeredNoise2dDesc);
                addfuns.addSymbol ("Heightmap", heightmapDesc);

                // libnoise integration
                addfuns.addSymbol ("LibnoisePerlin", libnoisePerlinDesc);
                addfuns.addSymbol ("LibnoiseBillow", libnoiseBillowDesc);
                addfuns.addSymbol ("LibnoiseCylinders", libnoiseCylindersDesc);
                addfuns.addSymbol ("LibnoiseRidgedMulti", libnoiseRidgedMultiDesc);
                addfuns.addSymbol ("LibnoiseSpheres", libnoiseSpheresDesc);
                addfuns.addSymbol ("LibnoiseVoronoi", libnoiseVoronoiDesc);
                //addfuns.addSymbol ("LibnoiseTurbulence", libnoiseTurbulenceDesc); //TODO: requires a source module (i.e. we should write a libnoise-module which accepts quatsch-code)

                // deprecated
                //addfuns.addSymbol ("LayeredNoise2d", layeredNoise2dDesc);
                return addfuns;
        }

        FunctionSet functionSet;
        Compiler::FunctionPtr fun;

public:
        typename BASE::return_type operator ()
         (real_t const & u, real_t const & v) const {
                //real_t const d = sqrt (u*u + v*v);
                const real_t p [] = { u, v };
                return typename BASE::return_type((*fun) (p));
        }


        quatsch_function (const std::string code)
        : functionSet(addfuns())
        {
                std::stringstream errors;
                fun = Compiler::compile (
                        "x;y",
                        //"(* 0.5 (* (sin (* x 2.0)) (sin(* y 2.0)) ))",
                        /*"(- 1 (abs ([LayeredNoise2d filter{cosine} seed{12} frequency{0.25} layercount{4} persistence{0.54} levelEvaluationFunction{(abs h)}] "
                        "  (+ y (^ (abs ([LayeredNoise2d filter{cosine} seed{12} frequency{0.25} layercount{12} persistence{0.54} levelEvaluationFunction{(abs h)}] x y)) 4))"
                        "  (+ y (^ (abs ([LayeredNoise2d filter{cosine} seed{12} frequency{0.25} layercount{12} persistence{0.54} levelEvaluationFunction{(abs h)}] x y)) 4))"
                        ")))",*/
                        code,
                        //"(* x 0.1)",
                        functionSet,
                        errors);
        }

        quatsch_function (const std::string code, std::stringstream &errors)
        : functionSet(addfuns())
        {
                fun = Compiler::compile (
                        "x;y",
                        //"(* 0.5 (* (sin (* x 2.0)) (sin(* y 2.0)) ))",
                        /*"(- 1 (abs ([LayeredNoise2d filter{cosine} seed{12} frequency{0.25} layercount{4} persistence{0.54} levelEvaluationFunction{(abs h)}] "
                        "  (+ y (^ (abs ([LayeredNoise2d filter{cosine} seed{12} frequency{0.25} layercount{12} persistence{0.54} levelEvaluationFunction{(abs h)}] x y)) 4))"
                        "  (+ y (^ (abs ([LayeredNoise2d filter{cosine} seed{12} frequency{0.25} layercount{12} persistence{0.54} levelEvaluationFunction{(abs h)}] x y)) 4))"
                        ")))",*/
                        code,
                        //"(* x 0.1)",
                        functionSet,
                        errors);
        }
};

typedef quatsch_function<HeightFunction> QuatschHeightFunction;
typedef quatsch_function<DistributionFunction> QuatschDistributionFunction;

} }

#endif // QUATSCH_HEIGHT_FUNCTION_HH_INCLUDED_20100227
