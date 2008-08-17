/***************************************************************************
 *            QuadtreeHeightField.cc
 *
 *  2008-08-08
 *  Copyright  2008  Sebastian Mach
 *  seb@greenhybrid.net
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <iostream>

#include <picogen/picogen.h>
#include <picogen/graphics/objects/QuadtreeHeightField.h>


typedef ::picogen::misc::prim::real real;
typedef ::picogen::misc::geometrics::Vector3d Vector3d;
typedef ::picogen::misc::geometrics::Ray Ray;
typedef ::picogen::misc::geometrics::BoundingBox BoundingBox;
typedef ::picogen::graphics::material::abstract::IBRDF IBRDF;
typedef ::picogen::graphics::material::abstract::IShader IShader;
typedef ::picogen::graphics::structs::intersection_t intersection_t;
typedef ::picogen::misc::functional::Function_R2_R1 Function_R2_R1;

/// \todo bring the following triangle intersector in some intersector-repository or something
static const real tri_eps = 0.00000001;
static int
raytri_intersect (
    const Ray &ray,
    const real a[], const real b[], const real c[],
    real &t, real &u, real &v,
    real normal[]
) {
    real vect0[3], vect1[3], nvect[3];
    real det, inv_det;


    //SUB(vect0, b,a)
    vect0[0] = b[0] - a[0];
    vect0[1] = b[1] - a[1];
    vect0[2] = b[2] - a[2];

    //SUB(vect1, c,a)
    vect1[0] = c[0] - a[0];
    vect1[1] = c[1] - a[1];
    vect1[2] = c[2] - a[2];

    //CROSS(normal, vect0, vect1);
    normal[0] = vect0[1] * vect1[2] - vect0[2] * vect1[1];
    normal[1] = vect0[2] * vect1[0] - vect0[0] * vect1[2];
    normal[2] = vect0[0] * vect1[1] - vect0[1] * vect1[0];

    /* orientation of the ray with respect to the triangle's normal,
       also used to calculate output parameters*/
    //det = - DOT(dir,normal);
    det = - (ray.w() [0] * normal[0] + ray.w() [1] * normal[1] + ray.w() [2] * normal[2]);

    //---------

    /* if determinant is near zero, ray is parallel to the plane of triangle */
    if (det > -tri_eps && det < tri_eps) return 0;

    /* calculate vector from ray origin to a */
    //SUB(vect0,a,orig);
    vect0[0] = a[0] - ray.x() [0];
    vect0[1] = a[1] - ray.x() [1];
    vect0[2] = a[2] - ray.x() [2];

    /* normal vector used to calculate u and v parameters */
    //CROSS(nvect,dir,vect0);
    //nvect = ray.w().cross( vect0 );
    nvect[0] = ray.w() [1] * vect0[2] - ray.w() [2] * vect0[1];
    nvect[1] = ray.w() [2] * vect0[0] - ray.w() [0] * vect0[2];
    nvect[2] = ray.w() [0] * vect0[1] - ray.w() [1] * vect0[0];

    inv_det = 1.0 / det;
    /* calculate vector from ray origin to b*/
    //SUB(vect1,b,orig);
    //vect1 = b - ray.x();
    vect1[0] = b[0] - ray.x() [0];
    vect1[1] = b[1] - ray.x() [1];
    vect1[2] = b[2] - ray.x() [2];

    /* calculate v parameter and test bounds */
    //*v = - DOT(vect1,nvect) * inv_det;
    //v = -(vect1*nvect*inv_det);
    v = -inv_det * (vect1[0] * nvect[0] + vect1[1] * nvect[1] + vect1[2] * nvect[2]);

    if (v < 0.0 || v > 1.0) return 0;

    /* calculate vector from ray origin to c*/
    //SUB(vect1,c,orig);
    vect1[0] = c[0] - ray.x() [0];
    vect1[1] = c[1] - ray.x() [1];
    vect1[2] = c[2] - ray.x() [2];

    /* calculate v parameter and test bounds */
    //*u = DOT(vect1,nvect) * inv_det;
    //u = vect1*nvect*inv_det;
    u = (vect1[0] * nvect[0] + vect1[1] * nvect[1] + vect1[2] * nvect[2]) * inv_det;

    if (u < 0.0 || u + v > 1.0) return 0;

    /* calculate t, ray intersects triangle */
    //*t = - DOT(vect0,normal) * inv_det;
    //t = -( vect0* normal * inv_det );
    t = -inv_det * (vect0[0] * normal[0] + vect0[1] * normal[1] + vect0[2] * normal[2]);

    //---------

    if (t < 0)
        return 0;
    const real tmp = 1.0 / sqrt (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] *= tmp;
    normal[1] *= tmp;
    normal[2] *= tmp;
    if (ray.w() [0]*normal[0] + ray.w() [1]*normal[1] + ray.w() [2]*normal[2] > 0.0)
        return -1;
    return 1;
}

typedef picogen::misc::prim::real real;
typedef picogen::misc::geometrics::Vector3d Vector3d;
typedef picogen::misc::geometrics::Ray Ray;
typedef picogen::misc::geometrics::BoundingBox BoundingBox;
typedef picogen::graphics::material::abstract::IBRDF IBRDF;
typedef picogen::graphics::structs::intersection_t intersection_t;

namespace picogen {
    namespace graphics {
        namespace objects {

            //=================================================================
            // Static Functions.
            //=================================================================

            inline real QuadtreeHeightField :: smoothedHeightFunc (
                const Function_R2_R1 &fun, real fu, real fv, real cellSizeU, real cellSizeV
            ) {
                const real su = cellSizeU * 0.5;
                const real sv = cellSizeV * 0.5;
                return
                    0.25*
                    (  fun (fu,   fv)
                     + fun (fu + su, fv)
                     + fun (fu,   fv + sv)
                     + fun (fu + su, fv + sv)
                    );
            }



            QuadtreeHeightField* QuadtreeHeightField :: create (
                unsigned int size,
                const Function_R2_R1 &fun,
                param_in (BoundingBox, bbox),
                real boundsGuessAccuracy, bool smooth
            ) {
                fprintf (stderr, "preparing QuadtreeHeightField {\n");
                fprintf (stderr, "  sizeof{Height,HeightChunk,Node}={%u,%u,%u}\n", sizeof (Height), sizeof (Chunk), sizeof (Node));

                unsigned int u, v;
                const real cellSize = 1.0 / (real) size;

                // get min and max value of function
                int updateDisplay = 32768;
                fprintf (stderr, "  determining bounds of height function %xP\n", reinterpret_cast<unsigned int> (&fun) );
                real h_min = misc::constants::real_max;
                real h_max = -misc::constants::real_max;
                if (boundsGuessAccuracy>0.9f) {
                    fprintf (stderr, "   doing this at full accuracy (requested was %.1f%%)\n", boundsGuessAccuracy*100.0f);
                    for (v = 0; v < size; v++) {
                        const real fv = v / (real) size;
                        for (u = 0; u < size; u++) {
                            //std::cout << "(" << u << ", " << v << ")";
                            const real fu = u / (real) size;
                            const real t = smooth ? smoothedHeightFunc (fun, fu, fv, cellSize, cellSize) : fun (fu, fv);
                            h_min = t < h_min ? t : h_min;
                            h_max = t > h_max ? t : h_max;
                        }
                        if( ++updateDisplay > 1000 ) {
                            updateDisplay = 0;
                            fprintf (stderr, "\r   %.4f%% [%u/%u]", 100.0f * static_cast<float> (u + v*size) / static_cast<float> (size*size), v, size);
                            fflush (stderr);
                        }
                        /*if( v%(size/10)==0 )
                         fprintf( stderr, "." );*/
                    }
                } else {
                    fprintf (stderr, "   using random sampling (requested %.1f%% of accuracy)\n", boundsGuessAccuracy*100.0f);
                    int numSamples = static_cast<int> (boundsGuessAccuracy*static_cast<float> (size*size));
                    for (int i=0; i<numSamples; ++i) {
                        const real fu = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
                        const real fv = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
                        const real t = smooth ? smoothedHeightFunc (fun, fu, fv, cellSize, cellSize) : fun (fu, fv);
                        h_min = t < h_min ? t : h_min;
                        h_max = t > h_max ? t : h_max;
                        if (++updateDisplay > 1000) {
                            updateDisplay = 0;
                            fprintf (stderr, "\r   %.4f%% [%u/%u]", 100.0f * static_cast<float> (i) / static_cast<float> (numSamples), i, numSamples);
                            fflush (stderr);
                        }
                    }
                }
                fprintf (stderr, "  \ndone: f(u,v)_{min=%.2f,max=%.2f}\n", static_cast<float> (h_min), static_cast<float> (h_max));
                fprintf (stderr, "  initializing QuadtreeHeightField ... ");
                QuadtreeHeightField * ret = new QuadtreeHeightField (size, fun, bbox, smooth, h_min, h_max);
                fprintf (stderr, "done\n");
                fprintf (stderr, "}\n");
                return ret;
            }



            void QuadtreeHeightField::initNode (
                QuadtreeHeightField::Node *node, QuadtreeHeightField::Node *parent,
                unsigned int left, unsigned int top, unsigned int size, const unsigned int minSize, const unsigned int heightFieldSize,
                bool smooth,
                const Function_R2_R1 &fun, real fun_min, real fun_max,
                real percentageFinished, real percentageFinishedScale
            ) {

                node->chunk.h_min.h = Height::max;
                node->chunk.h_max.h = Height::min;
                node->children = 0;

                const bool isLeaf = size <= 2 || size < minSize;
                if (!isLeaf) {
                    const unsigned int size05 = size >> 1;
                    node->children = new Node [4];

                    initNode (&node->children [0], node, left,        top,        size05, minSize, heightFieldSize, smooth, fun, fun_min, fun_max, percentageFinished, percentageFinishedScale*0.25);
                    percentageFinished += 0.25 * percentageFinishedScale;
                    initNode (&node->children [1], node, left+size05, top,        size05, minSize, heightFieldSize, smooth, fun, fun_min, fun_max, percentageFinished, percentageFinishedScale*0.25);
                    percentageFinished += 0.25 * percentageFinishedScale;
                    initNode (&node->children [2], node, left,        top+size05, size05, minSize, heightFieldSize, smooth, fun, fun_min, fun_max, percentageFinished, percentageFinishedScale*0.25);
                    percentageFinished += 0.25 * percentageFinishedScale;
                    initNode (&node->children [3], node, left+size05, top+size05, size05, minSize, heightFieldSize, smooth, fun, fun_min, fun_max, percentageFinished, percentageFinishedScale*0.25);
                    percentageFinished += 0.25 * percentageFinishedScale;

                    Height childMinHeight, childMaxHeight;
                    childMinHeight.h = Height::max;
                    childMaxHeight.h = Height::min;
                    for (unsigned int u=0; u<4; ++u) {
                        if (node->children [u].chunk.h_min.h < childMinHeight.h)
                            childMinHeight = node->children [u].chunk.h_min;
                        if (node->children [u].chunk.h_max.h > childMaxHeight.h)
                            childMaxHeight = node->children [u].chunk.h_max;
                    }

                    if (childMinHeight.h < node->chunk.h_min.h)
                        node->chunk.h_min.h = childMinHeight.h;
                    if (childMinHeight.h > node->chunk.h_max.h)
                        node->chunk.h_max.h = childMinHeight.h;

                    if (childMaxHeight.h < node->chunk.h_min.h)
                        node->chunk.h_min.h = childMaxHeight.h;
                    if (childMaxHeight.h > node->chunk.h_max.h)
                        node->chunk.h_max.h = childMaxHeight.h;
                } else {
                    if ((int)(percentageFinished*100000) % 100 == 0) {
                        fprintf (stderr, "\r   %.1f%%", 100.0f * percentageFinished);
                        fflush (stderr);
                    }
                }

                const unsigned int step = size / (Chunk::size-1);
                if ((size % (Chunk::size-1)) != 0 || size<1) {
                    std::cerr << "  bad node size ...\n" << std::endl;
                }

                const real cellSize = 1.0 / (real) heightFieldSize;

                for (unsigned int v=top, local_v=0; v<=top+size; v+=step, ++local_v) {
                    for (unsigned int u=left, local_u=0; u<=left+size; u+=step, ++local_u) {

                        const real fu = static_cast<real> (u) / static_cast<real> (heightFieldSize);
                        const real fv = static_cast<real> (v) / static_cast<real> (heightFieldSize);

                        const real fh_ = smooth ? smoothedHeightFunc (fun, fu, fv, cellSize, cellSize) : fun (fu, fv);
                        const real fh = (fh_-fun_min) / (fun_max-fun_min);
                        Height h;
                        h.fromReal (fh);

                        node->chunk.field [local_v*Chunk::size + local_u] = h;

                        if (isLeaf) {
                            node->chunk.h_min = h.h < node->chunk.h_min.h ? h : node->chunk.h_min;
                            node->chunk.h_max = h.h > node->chunk.h_max.h ? h : node->chunk.h_max;
                        }
                        if(false)if (0 != parent) {
                            std::cout << "<<" << parent->chunk.h_min.asReal() << " : " <<  parent->chunk.h_max.asReal() << "  ||  ";
                            parent->chunk.h_min = h.h < parent->chunk.h_min.h ? h : parent->chunk.h_min;
                            parent->chunk.h_max = h.h > parent->chunk.h_max.h ? h : parent->chunk.h_max;
                            std::cout << parent->chunk.h_min.asReal() << " : " <<  parent->chunk.h_max.asReal() << ">>\n";
                        }

                        if (false)if (0 == parent || isLeaf) {
                            std::cout << (isLeaf ? "leaf " : "root ");
                            std::cout << "(" << fh << "|" << h.asReal() << ")";
                            std::cout << "<" << h.asReal() << ":" <<  node->chunk.h_min.asReal() << "|" << node->chunk.h_max.asReal() << ">\n";
                        }
                    }
                }
            }


            //=================================================================
            // Non-Static Functions.
            //=================================================================

            QuadtreeHeightField :: QuadtreeHeightField (
                unsigned int size,
                const Function_R2_R1 &fun,
                const BoundingBox &bbox,
                bool smooth,
                real h_min, real h_max
            ) {
                std::cout << "\n";
                initNode (
                    &rootNode, 0,  // node, parent-node
                    0, 0, size, // left, top, size
                    2, size, // min size, max size
                    smooth,
                    fun, h_min, h_max,
                    0.0, 1.0
                );

                bboxSize [0] = bbox.computeWidth ();
                bboxSize [1] = bbox.computeHeight ();
                bboxSize [2] = bbox.computeDepth ();

                bboxMin [0] = bbox.getMin() [0];
                bboxMin [1] = bbox.getMin() [1];
                bboxMin [2] = bbox.getMin() [2];

                std::cerr << "  bboxSize={" << bboxSize[0] << ", " << bboxSize[1] << ", " << bboxSize[2] << ")\n";
                std::cerr << "  bboxMin={" << bboxMin[0] << ", " << bboxMin[1] << ", " << bboxMin[2] << ")\n";
                std::cerr << "  h_{min|max}={" << h_min << ", " << h_max  << ")\n";
                std::cerr << "  rootNode.h_{Min|Max}={" << rootNode.chunk.h_min.asReal() << ", " << rootNode.chunk.h_max.asReal() << "}\n";

                heightFieldSize = size;
            }



            QuadtreeHeightField::~QuadtreeHeightField() {
            }



            bool QuadtreeHeightField::intersectNode (
                param_out (intersection_t, intersection),
                param_in (Ray, ray), param_in (QuadtreeHeightField::Node, node),
                const unsigned int left, const unsigned int top, const unsigned int size
            ) const {
                using namespace misc::constants;

                // TODO: avoid casting
                const real fu_ = static_cast<real> (left) / static_cast<real> (heightFieldSize);
                const real fu  = fu_ * bboxSize [0] + bboxMin [0];
                const real fv_ = static_cast<real> (top) / static_cast<real> (heightFieldSize);
                const real fv  = fv_ * bboxSize [2] + bboxMin [2];

                const real fu1_ = static_cast<real> (size+left) / static_cast<real> (heightFieldSize);
                const real fu1  = fu1_ * bboxSize [0] + bboxMin [0];
                const real fv1_ = static_cast<real> (size+top) / static_cast<real> (heightFieldSize);
                const real fv1  = fv1_ * bboxSize [2] + bboxMin [2];

                const real fchunkMin = node.chunk.h_min.asReal() * bboxSize [1] + bboxMin [1];
                const real fchunkMax = node.chunk.h_max.asReal() * bboxSize [1] + bboxMin [1];

                const BoundingBox localBox (
                    Vector3d (fu, fchunkMin, fv),
                    Vector3d (fu1, fchunkMax, fv1)
                );
                real boxHitMin, boxHitMax;
                if (!localBox.intersect (boxHitMin, boxHitMax, ray)) {
                    return false;
                }

                bool sign_x = ray.w()[0] > 0.0; // TODO: precalculate
                bool sign_z = ray.w()[2] > 0.0; // TODO: precalculate

                if (0 == node.children) {

                    int step_x = sign_x ? 1 : -1;
                    int step_y = sign_z ? 1 : -1;
                    int start_x = sign_x ? 0 : Chunk::size-2;
                    int start_y = sign_z ? 0 : Chunk::size-2;
                    int end_x = sign_x ? Chunk::size-1 : -1;
                    int end_y = sign_z ? Chunk::size-1 : -1;

                    intersection.t = real_max;
                    for (int y=start_y; y!=end_y; y+=step_y) {
                        for (int x=start_x; x!=end_x; x+=step_x) {
                            const real u1 = static_cast<real>(x) / static_cast<real>(Chunk::size-1);
                            const real v1 = (static_cast<real>(y) / static_cast<real>(Chunk::size-1));
                            const real u2 = static_cast<real>(x+1) / static_cast<real>(Chunk::size-1);
                            const real v2 = (static_cast<real>(y+1) / static_cast<real>(Chunk::size-1));

                            const real left   = (1-u1)*fu + (u1)*fu1;
                            const real right  = (1-u2)*fu + (u2)*fu1;
                            const real top    = (1-v1)*fv + (v1)*fv1;
                            const real bottom = (1-v2)*fv + (v2)*fv1;

                            const Height ah = node.chunk.field [(y+1)*Chunk::size + x];
                            const Height bh = node.chunk.field [y*Chunk::size + x];
                            const Height ch = node.chunk.field [y*Chunk::size + (x+1)];
                            const Height dh = node.chunk.field [(y+1)*Chunk::size + (x+1)];

                            const real a[3] = { left, ah.asReal()*bboxSize[1]+bboxMin[1], bottom };
                            const real b[3] = { left, bh.asReal()*bboxSize[1]+bboxMin[1], top };
                            const real c[3] = { right, ch.asReal()*bboxSize[1]+bboxMin[1], top };
                            const real d[3] = { right, dh.asReal()*bboxSize[1]+bboxMin[1], bottom };

                            bool didHit = false;
                            real tmp_t, tmp_u, tmp_v, tmp_n[3];
                            int hit;
                            if (0 != (hit=raytri_intersect (ray, a, c, b, tmp_t, tmp_u, tmp_v, tmp_n)) && tmp_t<intersection.t) {
                                intersection.normal = hit>0?Vector3d (tmp_n[0], tmp_n[1], tmp_n[2]) : Vector3d (-tmp_n[0], -tmp_n[1], -tmp_n[2]);
                                intersection.t = tmp_t;
                                didHit = true;
                            }
                            if (0 != (hit=raytri_intersect (ray, a, d, c, tmp_t, tmp_u, tmp_v, tmp_n)) && tmp_t<intersection.t) {
                                intersection.normal = hit>0?Vector3d (tmp_n[0], tmp_n[1], tmp_n[2]) : Vector3d (-tmp_n[0], -tmp_n[1], -tmp_n[2]);
                                intersection.t = tmp_t;
                                didHit = true;
                            }

                            if (intersection.t != real_max) {
                                intersection.side = outside;
                                intersection.color = image::color::Color (1,1,1);
                                return true;
                            }
                        }
                    }

                    return false;

                } else {

                    const unsigned int size05 = size >> 1;
                    const unsigned int coords [4][2] = {
                        {left, top},
                        {left+size05, top},
                        {left, top+size05},
                        {left+size05, top+size05}
                    };

                    // Find traversal order.
                    unsigned int order [4];

                    if (sign_x && !sign_z) {
                        order [0] = 2;
                        order [1] = 3;
                        order [2] = 0;
                        order [3] = 1;
                    } else if (!sign_x && !sign_z) {
                        order [0] = 3;
                        order [1] = 2;
                        order [2] = 1;
                        order [3] = 0;
                    } else if (sign_x && sign_z) {
                        order [0] = 0;
                        order [1] = 1;
                        order [2] = 2;
                        order [3] = 3;
                    } else if (!sign_x && sign_z) {
                        order [0] = 1;
                        order [1] = 0;
                        order [2] = 3;
                        order [3] = 2;
                    }


                    intersection_t tmp;
                    for (unsigned int u=0; u<4; ++u) {
                        if (intersectNode (tmp, ray, node.children [order [u]], coords [order [u]][0], coords [order [u]][1], size05)) {
                            intersection = tmp;
                            return true;
                        }
                    }
                    return false;
                }
            }



            bool QuadtreeHeightField::intersect (param_out (intersection_t, intersection), param_in (Ray, ray)) const {
                if (intersectNode (intersection, ray, rootNode, 0, 0, heightFieldSize)) {
                    if(false) {
                        if (0 != shader) {
                            shader->shade (
                                intersection.color,
                                intersection.normal,
                                ray.x() + ray.w() * intersection.t
                            );
                        } else {
                            intersection.color = image::color::Color (1.0, 1.0, 1.0);
                        }
                    }
                    intersection.brdf = brdf;
                    intersection.L_e = 0.0;
                    return true;
                }
                return false;
            }



            /*void QuadtreeHeightField::setBox (param_in (Vector3d, min), param_in (Vector3d, max)) {
                bbox.setMin (min);
                bbox.setMax (max);
            }*/



            void QuadtreeHeightField::setBRDF (const IBRDF* brdf) {
                this->brdf = brdf;
            }



            void QuadtreeHeightField::setShader (const IShader* shader) {
                this->shader = shader;
            }

        }; // namespace objects
    }; // namespace graphics
}; // namespace picogen

/*
namespace picogen {
    namespace graphics {
        namespace objects {



            QuadtreeHeightField::QuadtreeHeightField (unsigned int maxSize, SimpleHeightField *simpleHeightField)
            : field (simpleHeightField)
            , rootBox (simpleHeightField->getBoundingBox())
            , rootNode (QuadtreeNode::create (
                buildChildren (0, 0, simpleHeightField->getSize(), maxSize, rootBox, 0), rootBox,
                    0, 0, maxSize, maxSize
                )
            )
            {
            }



            QuadtreeHeightField::~QuadtreeHeightField() {
                if (0 != field)
                    delete field;
                if (0 != rootNode)
                    delete rootNode;
            }



            bool QuadtreeHeightField::intersect (param_out (intersection_t, intersection), param_in (Ray, ray)) const {
                return rootNode->intersect (intersection, *field, ray);
            }



            QuadtreeHeightField::QuadtreeNode *
            QuadtreeHeightField::buildChildren
            ( unsigned int u, unsigned int v,
              unsigned int size,
              const unsigned int maxSize,
              const BoundingBox &rootBox,
              BoundingBox *aggregateBB
            ) const {

                static unsigned int innerNodeId=0;

                const unsigned int heightfieldSize = field->getSize();
                const unsigned maxRight  = u+size >= heightfieldSize ? heightfieldSize-1 : u+size;
                const unsigned maxBottom = v+size >= heightfieldSize ? heightfieldSize-1 : v+size;

                if (size<maxSize) {
                    // init aggregate here
                    const real rootSize [3] = {
                        rootBox.computeWidth(),
                        rootBox.computeHeight(),
                        rootBox.computeDepth()
                    };

                    const real rootMin [3] = {
                        rootBox.getMin() [0],
                        rootBox.getMin() [1],
                        rootBox.getMin() [2],
                    };

                    const unsigned int &left = u;
                    const unsigned int &top = v;

                    const unsigned right  = maxRight;//left+size >= heightfieldSize ? heightfieldSize-1 : left+size;
                    const unsigned bottom = maxBottom;//top+size >= heightfieldSize ? heightfieldSize-1 : top+size;

                    if (true) {
                        std::cout << " <<" << left << ", " << top << ", " << right << ", " << bottom << ">>" << std::endl;
                    }

                    if (0 != aggregateBB)
                        aggregateBB->reset();

                    for (unsigned int u=left; u<=right; ++u) {
                        for (unsigned int v=top; v<=bottom; ++v) {

                            const real fu = static_cast<real> (u) / static_cast<real> (heightfieldSize);
                            const real fw = SimpleHeightField::htor (field->getHeight (u, v));
                            const real fv = static_cast<real> (v) / static_cast<real> (heightfieldSize);

                            const Vector3d worldPosition (
                                rootSize[0] * fu + rootMin[0],
                                rootSize[1] * fw + rootMin[1],
                                rootSize[2] * fv + rootMin[2]
                            );

                            if (0 != aggregateBB)
                                aggregateBB->update (worldPosition);
                        }
                    }

                    if (false) {
                        static unsigned int counter = 0;
                        if (0 != aggregateBB) {
                            const real s [3] = {
                                aggregateBB->computeWidth(),
                                aggregateBB->computeHeight(),
                                aggregateBB->computeDepth()
                            };
                            std::cout << "(" << innerNodeId << "." << counter << ")";
                            std::cout << " size={" << s[0] << "," << s[1] << "," << s[2] << "}";
                            std::cout << " min={" << aggregateBB->getMin() [0] << "," << aggregateBB->getMin() [1] << "," << aggregateBB->getMin() [2] << "}";
                            std::cout << " max={" << aggregateBB->getMax() [0] << "," << aggregateBB->getMax() [1] << "," << aggregateBB->getMax() [2] << "}";
                            std::cout << "\n";
                        }
                        ++counter;
                    }
                    return 0;
                } else {
                    QuadtreeNode *children = QuadtreeNode::createArray(4);

                    const unsigned int halfSize = size>>1;

                    BoundingBox boxes [4];
                    QuadtreeNode *tmp [4] = {
                        buildChildren (u,          v+halfSize, halfSize, maxSize, rootBox, &boxes [0]),
                        buildChildren (u+halfSize, v+halfSize, halfSize, maxSize, rootBox, &boxes [1]),
                        buildChildren (u,          v,          halfSize, maxSize, rootBox, &boxes [2]),
                        buildChildren (u+halfSize, v,          halfSize, maxSize, rootBox, &boxes [3])
                    };

                    //const unsigned int maxRight  = u+halfSize+halfSize >= heightfieldSize ? heightfieldSize-1 : u+halfSize+halfSize;
                    //const unsigned int maxBottom = v+halfSize+halfSize >= heightfieldSize ? heightfieldSize-1 : v+halfSize+halfSize;

                    children [0].setRectangle (u,          v+halfSize, u+halfSize,          maxBottom );
                    children [1].setRectangle (u+halfSize, v+halfSize, maxRight,            maxBottom );
                    children [2].setRectangle (u,          v,          u+halfSize,          v+halfSize          );
                    children [3].setRectangle (u+halfSize, v,          maxRight,            v+halfSize          );

                    if (0 == tmp [0]) {
                        std::cout << "{" << std::endl;
                        std::cout << " ( " << u          <<", "<< v+halfSize<<", "<< u+halfSize         << ", " << maxBottom <<")"<<  std::endl;
                        std::cout << " ( " << u+halfSize <<", "<< v+halfSize<<", "<< maxRight<< ", " << maxBottom <<")"<<  std::endl;
                        std::cout << " ( " << u          <<", "<< v         <<", "<< u+halfSize         << ", " << v+halfSize          <<")"<<  std::endl;
                        std::cout << " ( " << u+halfSize <<", "<< v         <<", "<< maxRight << ", " << v+halfSize          <<")"<<  std::endl;
                        std::cout << "}" << std::endl;
                    }

                    if (0 != aggregateBB)
                        aggregateBB->reset();
                    for (unsigned int u=0; u<4; ++u) {
                        children [u].setBoundingBox (boxes [u]);
                        children [u].setChildren (tmp [u]);
                        if (0 != aggregateBB) {
                            aggregateBB->update (boxes [u].getMin());
                            aggregateBB->update (boxes [u].getMax());
                        }
                    }

                    ++innerNodeId;
                    return children;
                }
                return 0;
            }



        }; // namespace objects
    }; // namespace graphics
}; // namespace picogen
*/