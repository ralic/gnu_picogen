//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2009  Sebastian Mach (*1983)
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

#ifndef MATCH_DESCRIPTORS_HH_INCLUDED_20090918
#define MATCH_DESCRIPTORS_HH_INCLUDED_20090918

#include <list>

namespace actuarius { namespace detail {
        template <typename iterator_t>
        struct match_t {
                match_t (iterator_t begin, iterator_t end)
                : begin_(begin)
                , end_(end)
                , success_ (true)
                {}

                match_t ()
                : success_ (false)
                {}

                match_t (match_t const & rhs)
                : begin_ (rhs.begin_)
                , end_(rhs.end_)
                , success_ (rhs.success_)
                {}

                match_t &operator = (match_t const & rhs) {
                        begin_   = rhs.begin_;
                        end_     = rhs.end_;
                        success_ = rhs.success_;
                        return *this;
                }

                iterator_t begin () const {
                        return begin_;
                }

                iterator_t end () const {
                        return end_;
                }

                bool success () const {
                        return success_;
                }

                operator bool () const {
                        return success_;
                }


        private:
                iterator_t begin_;
                iterator_t end_;
                bool success_;
        };



        template <typename iterator_t>
        struct block_match_t {

                block_match_t (
                        match_t<iterator_t> id,
                        match_t<iterator_t> content
                )
                : id_(id)
                , content_(content)
                , success_ (true)
                {
                }

                block_match_t (
                        match_t<iterator_t> content
                )
                : content_(content)
                , success_ (true)
                {}

                block_match_t ()
                : success_ (false)
                {}

                block_match_t (block_match_t const & rhs)
                : id_ (rhs.id_)
                , content_(rhs.content_)
                , success_ (rhs.success_)
                {}

                block_match_t &operator = (block_match_t const & rhs) {
                        id_      = rhs.id_;
                        content_ = rhs.content_;
                        success_ = rhs.success_;
                        return *this;
                }

                operator bool () const {
                        return success_;
                }

                iterator_t content_end () const {
                        return content_.end();
                }

                match_t<iterator_t> id () const {
                        return id_;
                }

                match_t<iterator_t> content () const {
                        return content_;
                }

                bool success () const {
                        return success_;
                }

        private:
                match_t<iterator_t> id_;
                match_t<iterator_t> content_;
                bool success_;

        };

        template <typename iterator_t>
        struct value_match_t {
                value_match_t (
                        match_t<iterator_t> id,
                        match_t<iterator_t> value
                )
                : id_(id)
                , value_(value)
                , success_ (true)
                {}



                value_match_t (
                        match_t<iterator_t> value
                )
                : value_(value)
                , success_ (true)
                {}


                value_match_t ()
                : success_ (false)
                {}


                value_match_t (value_match_t const & rhs)
                : id_ (rhs.id_)
                , value_(rhs.value_)
                , success_ (rhs.success_)
                {}


                value_match_t &operator = (value_match_t const &rhs) {
                        id_      = rhs.id_;
                        value_   = rhs.value_;
                        success_ = rhs.success_;
                        return *this;
                }


                operator bool () const {
                        return success_;
                }


                iterator_t value_end () const {
                        return value_.end();
                }


                match_t<iterator_t> id () const {
                        return id_;
                }


                match_t<iterator_t> value () const {
                        return value_;
                }


                bool success () const {
                        return success_;
                }

        private:
                match_t<iterator_t> id_;
                match_t<iterator_t> value_;
                bool success_;
        };



        template <typename iterator_t>
        class block_t {
                typedef value_match_t<iterator_t> vm_t;
        public:
                block_t (block_match_t<iterator_t> const &block)
                : valid (true)
                , is_top_level_node (false)
                , block (block)
                , children ()
                , values ()
                {}

                block_t ()
                : valid (false)
                {}

                block_t (block_t const &rhs)
                : valid (rhs.valid)
                , is_top_level_node(rhs.is_top_level_node)
                , block (rhs.block)
                , children (rhs.children)
                , values (rhs.values)
                {}

                block_t & operator = (block_t const &rhs) {
                        valid    = rhs.valid;
                        is_top_level_node = rhs.is_top_level_node;
                        block    = rhs.block;
                        children = rhs.children;
                        values   = rhs.values;
                        return *this;
                }

                void add_child (block_t const &rhs) {
                        children.push_back (rhs);
                }

                block_t take_child (std::string path) {

                        const size_t s = path.find_first_of ('/');
                        const std::string uno = path.substr (0, s);
                        const std::string dos =  (s == std::string::npos)
                                ? ""
                                : path.substr (1+s);


                        for (typename std::list <block_t>::iterator
                             it  = children.begin();
                             it != children.end();
                             ++it
                        ) {
                                if (uno == it->id()) {
                                        if (dos.length() > 0) {
                                                return it->take_child (dos);
                                        } else {
                                                block_t ret = *it;
                                                children.erase (it);
                                                return ret;
                                        }
                                }
                        }

                        return block_t();
                }

                block_t take_first_child () {
                        if (children.size()) {
                                block_t ret = *children.begin();
                                children.pop_front();
                                return ret;
                        }
                        return block_t();
                }

                value_match_t<iterator_t> take_value (std::string name) {
                        typedef value_match_t<iterator_t>  vm_t;
                        for (typename std::list<vm_t>::iterator
                                it = values.begin();
                                it != values.end();
                                ++it
                        ) {
                                if (name == unescape_nonstring_terminal (
                                                it->id()
                                )) {
                                        value_match_t<iterator_t> ret = *it;
                                        values.erase (it);
                                        return ret;
                                }
                        }

                        return value_match_t<iterator_t>();
                }

                value_match_t<iterator_t> take_first_value() {
                        if (values.size()) {
                                value_match_t<iterator_t> ret = *values.begin();
                                values.pop_front();
                                return ret;
                        }
                        return value_match_t<iterator_t>();
                }

                bool take_first_value_or_child (
                        value_match_t<iterator_t> &vm, block_t &block
                ) {
                        if (vm = take_first_value()) return true;
                        if (block = take_first_child()) return true;
                        return false;
                }

                void add_value (value_match_t<iterator_t> const &rhs) {
                        values.push_back (rhs);
                }

                size_t child_count() const {
                        return children.size();
                }

                size_t value_count() const {
                        return values.size();
                }

                operator bool () const {
                        return valid;
                }

                std::string id () const {
                        return std::string (
                                block.id().begin(), block.id().end()
                        );
                }

                void dump (int r=0) {
                        if (r == 0)
                                std::cout << "-- 'TIS A DUMP ----------\n";
                        for (int i=0; i<r; i++)
                                std::cout << "  ";
                        std::cout << id() << std::endl;
                        for (typename std::list <block_t>::iterator
                                it = children.begin();
                                it != children.end();
                                ++it
                        ) {
                                it->dump (r+1);
                        }
                        for (int i=0; i<r; i++)
                                std::cout << "  ";

                        std::cout << "  value_count:" << value_count() << "\n";

                        for (typename std::list<vm_t>::const_iterator it=values.begin();
                             it!=values.end();
                             ++it
                        ) {
                                for (int i=0; i<r; i++)
                                        std::cout << " ";
                                std::cout << "    " <<
                                   std::string(it->value().begin(), it->value().end()) << "\n";
                        }
                        if (r == 0) {
                                std::cout << "-------------------------\n"
                                          << std::flush;
                        }
                }

                void set_as_top_level_node (bool set) {
                        is_top_level_node = set;
                }

        private:

                bool                        valid;
                bool                        is_top_level_node;
                block_match_t<iterator_t>   block;
                std::list <block_t>         children;
                std::list <vm_t>            values;
        };
} }

#endif // MATCH_DESCRIPTORS_HH_INCLUDED_20090918
