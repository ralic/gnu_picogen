#include "renderwidget.h"
#include "ui_renderwidget.h"
#include "glimpse/stopwatch.hh"
#include <QImage>

// Some features we want:
// * renderer with pause+continue capabilities
// * flexible, yet performant
// * clean, well-engineered code
namespace crystal {

        class RenderControl {
        public:
                // pause is expected to have low latency,
                // so canPause would return false e.g. in case an acceleration
                // structure must be built
                bool pause();
                bool canPause() const;

                void proceed();

                int activeThreadCount() const;
        };
}


#include "crystal_math.h"
#include "film.h"
#include "cameras/pinhole.h"
#include "geoblocks/ray_tri_intersect.h"
#include "geometry/terrain2d.h"
#include "surfaceintegrators/constant.h"
#include "surfaceintegrators/whitted.h"

#include "background/utah-sun.h"
#include "background/utah-sky.h"
#include "background/utah-atmosphere.h"
#include "background/utah-sky/sunsky.hh"

#include "scene.h"

#include "quatsch-height-function.h"

#include <boost/optional.hpp>

namespace crystal {
        class SurfaceIntegrator;
        class VolumeIntegrator;
        class Volume;

        class PixelShader {
        public:
                Radiance operator() (Radiance const &rad) const {
                        return this->shade_ (rad);
                }
        private:
                virtual Radiance shade_ (Radiance const &) const = 0;
        };
        namespace pixel_shaders {
                class Scale : public PixelShader {
                public:
                        Scale() : factor_ (1) {}
                        Scale (real factor) : factor_ (factor) {}

                private:
                        Radiance shade_ (Radiance const &rad) const {
                                return rad * factor_;
                        }

                        real factor_;
                };
        }
        class SurfaceShader {
        };


        class Renderer {
        public:
                virtual ~Renderer() {}

                void render() const { return this->render_(); }
            /*virtual void Render(const Scene *scene) = 0;
            virtual Spectrum Li(const Scene *scene, const RayDifferential &ray,
                const Sample *sample, RNG &rng, MemoryArena &arena,
                Intersection *isect = NULL, Spectrum *T = NULL) const = 0;
            virtual Spectrum Transmittance(const Scene *scene,
                const RayDifferential &ray, const Sample *sample,
                RNG &rng, MemoryArena &arena) const = 0;
                */

        private:
                virtual void render_() const = 0;

        };

        class FlatRenderer : public Renderer {
        public:
                FlatRenderer (shared_ptr<Film> film,
                              shared_ptr<const Scene> scene,
                              shared_ptr<const Camera> camera,
                              shared_ptr<const SurfaceIntegrator> surfaceIntegrator,
                              shared_ptr<const VolumeIntegrator>  volumeIntegrator,
                              shared_ptr<const PixelShader> pixelShader)
                        : film_(film)
                        , scene_(scene)
                        , camera_(camera)
                        , surfaceIntegrator_(surfaceIntegrator)
                        , volumeIntegrator_(volumeIntegrator)
                        , pixelShader_ (pixelShader)
                {
                }
        private:
                void render_ () const {
                        const int     height  = film_->height();
                        const int     width   = film_->width();
                        const Camera& camera  = *camera_;
                        const Scene&  scene   = *scene_;
                        const SurfaceIntegrator &surfaceInteg = *surfaceIntegrator_;
                        Film&         film    = *film_;
                        const PixelShader& ps = *pixelShader_;

                        for (int y=0; y<height; ++y) {
                                for (int x=0; x<width; ++x) {
                                        const CameraSample sample(x, y,
                                                                  x/real(width),
                                                                  y/real(height));
                                        const Ray ray = camera(sample);
                                        const Radiance rad = surfaceInteg(ray, scene);
                                        const Radiance shaded = ps (rad);
                                        film.addSample (sample, shaded);
                                }
                        }
                }

        private:
                shared_ptr<Film>                    film_;
                shared_ptr<const Scene>             scene_;
                shared_ptr<const Camera>            camera_;
                shared_ptr<const SurfaceIntegrator> surfaceIntegrator_;
                shared_ptr<const VolumeIntegrator>  volumeIntegrator_;
                shared_ptr<const PixelShader>       pixelShader_;
        };
}





RenderWidget::RenderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderWidget)
{
    ui->setupUi(this);
    on_resetsceneButton_clicked();
}

RenderWidget::~RenderWidget()
{
    delete ui;
}

void RenderWidget::on_pushButton_clicked()
{
        updateDisplay();
}

void RenderWidget::updateDisplay () {

        using namespace crystal;

        glimpse::StopWatch sw;


        sw.restart();
        shared_ptr<Film>           film     (new Film(320, 320));
        shared_ptr<const Camera>   camera   (new cameras::Pinhole(0.7));

        crystal::quatsch::height_function qh ("(* 100 ([LibnoisePerlin   "
                                              "         frequency{0.003} "
                                              "         octave-count{8}  "
                                              "         ] x y))          "
                                              //"(* 15 (sin (* 0.025 x))"
                                              //"      (sin (* 0.025 y)))"
                                             );
        std::function<real(real,real)> h = qh;

        shared_ptr<const Geometry> geometry (new geometry::Terrain2d(
                                                geometry::terrain2d::Deepness(
                                                        7, 200,3000
                                                ),
                                                h
                                                /*[](real x, real z) {
                                                     return 15*std::sin(0.025*x)
                                                             *std::sin(0.025*z); }*/
                                            ) );

        shared_ptr<const SurfaceIntegrator> surface_integrator (
                                                new surfaceintegrators::Whitted());

        shared_ptr<const redshift::background::PssSunSky> pssSunSky (
                                new redshift::background::PssSunSky(redshift::Vector(1,0.4,0),
                                                                    4,
                                                                    0,
                                                                    true));
        shared_ptr<const Scene> scene(new Scene(
                geometry,
                shared_ptr<const background::UtahSun> (new background::UtahSun(pssSunSky)),
                shared_ptr<const background::UtahSky> (new background::UtahSky(pssSunSky)),
                shared_ptr<const background::UtahAtmosphere> (new background::UtahAtmosphere(pssSunSky))
                ));

        shared_ptr<const Renderer> renderer (new FlatRenderer(
                film,
                scene,
                camera,
                surface_integrator,
                shared_ptr<const VolumeIntegrator>(),
                shared_ptr<const PixelShader>(new pixel_shaders::Scale(0.000075))
        ));
        const double creationTime = sw.stop();
        sw.restart();
        renderer->render();
        const double renderTime = sw.stop();


        const unsigned int width = film->width(),
                           height = film->height();
        QImage image (width, height, QImage::Format_RGB32);

        for (unsigned int y=0; y<height; ++y) {
                for (unsigned int x=0; x<width; ++x) {
                        const rgb_t rgb = static_cast<rgb_t> ((*film)(x,y));
                        const int r_ = rgb.r * 255,
                                  g_ = rgb.g * 255,
                                  b_ = rgb.b * 255;
                        const int r = r_<0?0:r_>255?255:r_,
                                  g = g_<0?0:g_>255?255:g_,
                                  b = b_<0?0:b_>255?255:b_;
                        image.setPixel (x, y,
                                        QColor(r,g,b).rgb());
                }
        }

        ui->label->setScaledContents(false);
        ui->label->setPixmap(QPixmap::fromImage(image));
        ui->perf->setText (QString("creation time: ") + QString::number(creationTime, 'g', 2)
                        + " sec, render time: " + QString::number(renderTime, 'g', 2)
                        + " sec");
        repaint();
        QApplication::processEvents();
}

void RenderWidget::on_resetsceneButton_clicked() {
        on_pushButton_clicked();
}

#include "renderwidget.moc"
