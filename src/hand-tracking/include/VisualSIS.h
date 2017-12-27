#ifndef VISUALSIS_H
#define VISUALSIS_H

#include <chrono>
#include <deque>
#include <memory>
#include <vector>

#include <BayesFilters/EstimatesExtraction.h>
#include <BayesFilters/VisualParticleFilter.h>
#include <Eigen/Dense>
#include <iCub/iKin/iKinFwd.h>
#include <opencv2/cudaobjdetect.hpp>
#include <thrift/VisualSISParticleFilterIDL.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IAnalogSensor.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Port.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>


class VisualSIS: public bfl::VisualParticleFilter,
                 public VisualSISParticleFilterIDL
{
public:
    VisualSIS(const yarp::os::ConstString& cam_sel,
              const int num_particles,
              const double resample_ratio);

    VisualSIS(const VisualSIS& vsir_pf) = delete;

    VisualSIS(VisualSIS&& vsir_pf) noexcept = delete;

    ~VisualSIS() noexcept;

    VisualSIS& operator=(const VisualSIS& vsir_pf) = delete;

    VisualSIS& operator=(VisualSIS&& vsir_pf) noexcept = delete;

    bool skip(const std::string& what_step, const bool status) override;

protected:
    void initialization() override;

    void filteringStep() override;

    void getResult() override;

    bool runCondition() override { return true; };


    yarp::os::ConstString cam_sel_;
    const int             num_particles_;
    double                resample_ratio_;

    cv::Ptr<cv::cuda::HOG> cuda_hog_;


    yarp::os::BufferedPort<yarp::sig::Vector>                       port_estimates_out_;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> port_image_in_;


    yarp::os::Port port_rpc_command_;
    bool           attach(yarp::os::Port &source);
    bool           setCommandPort();


    bool                     run_filter() override;

    bool                     reset_filter() override;

    bool                     stop_filter() override;

    bool                     skip_step(const std::string& what_step, const bool status) override;

    bool                     use_analogs(const bool status) override;

    std::vector<std::string> get_info() override;

    bool                     quit() override;


    bool set_estimates_extraction_method(const std::string& method) override;

    bool set_mobile_average_window(const int16_t window) override;

private:
    Eigen::MatrixXf pred_particle_;
    Eigen::VectorXf pred_weight_;

    Eigen::MatrixXf cor_particle_;
    Eigen::VectorXf cor_weight_;


    bfl::EstimatesExtraction estimate_extraction_;


    const int          block_size_        = 16;
    const int          img_width_         = 320;
    const int          img_height_        = 240;
    const int          bin_number_        = 9;
    const unsigned int descriptor_length_ = (img_width_/block_size_*2-1) * (img_height_/block_size_*2-1) * bin_number_ * 4;


    bool init_img_in_ = false;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img_in_;
};

#endif /* VISUALSIS_H */
