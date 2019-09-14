#include "simulation/windows/drivetrains.h"

#include <frc/RobotController.h>
#include <algorithm>

using namespace simulation;
using namespace curtinfrc;

void drivetrain_window::init() {
  static Usage<DrivetrainConfig>::Registry<drivetrain_window> registry(&ui::init_window<drivetrain_window, DrivetrainConfig *>);
}

drivetrain_window::drivetrain_window(DrivetrainConfig *config) : ui::window("Drivetrain", 600, 600), _config(config), physics_aware() {
  _enc_sim_left = components::create_encoder(config->leftDrive.encoder);
  _enc_sim_right = components::create_encoder(config->rightDrive.encoder);

  register_button(resetPos);
  register_button(scalePlus);
  register_button(scaleMinus);

  resetPos.set_can_activate(false);
  resetPos.on_click([&](bool, ui::button&) {
    _x = 0;
    _y = 0;
  });

  scaleMinus.set_can_activate(false);
  scaleMinus.on_click([&](bool, ui::button&) {
    scale = 1 / (1 / scale + 1);
  });

  scalePlus.set_can_activate(false);
  scalePlus.on_click([&](bool, ui::button&) {
    if (1 / scale > 1)
      scale = 1 / (1 / scale - 1);
  });
}

double drivetrain_window::get_motor_val(bool left) {
  auto &trans = left ? _config->leftDrive.transmission : _config->rightDrive.transmission;
  return trans->Get() * (trans->GetInverted() ? -1 : 1);
}

void drivetrain_window::add_encoder_position(bool left, double pos) {
  double C = 2 * 3.14159265 * _config->wheelRadius;
  double rots = pos / C;
  
  auto sim_encoder = left ? _enc_sim_left : _enc_sim_right;
  auto encoder = left ? _config->leftDrive.encoder : _config->rightDrive.encoder;

  if (encoder != nullptr) {
    sim_encoder->set_counts(static_cast<int>(encoder->GetEncoderTicks() + rots * encoder->GetEncoderTicksPerRotation()));
  }
}

void drivetrain_window::update_physics(double time_delta) {
  for (bool left : {true, false}) {
    wheel_state &state = left ? _wheel_left : _wheel_right;
    double speed = get_motor_val(left) * (left ? -1 : 1);
    double voltage = speed * frc::RobotController::GetInputVoltage();

    physics::DcMotor motor = left ? _config->leftDrive.motor : _config->rightDrive.motor;
    motor = motor.reduce(left ? _config->leftDrive.reduction : _config->rightDrive.reduction);

    double current = motor.current(voltage, state.angular_vel);
    double torque = motor.torque(current);
    double accel = torque / (_config->mass / 2.0 * _config->wheelRadius);

    state.angular_vel += (accel * time_delta) / _config->wheelRadius;
    state.angular_position += state.angular_vel * time_delta;

    add_encoder_position(left, state.angular_vel * time_delta);
  }

  double robot_angular_vel = (_wheel_right.angular_vel - _wheel_left.angular_vel) * _config->wheelRadius / _config->trackwidth;
  _linear_vel = (_wheel_left.angular_vel + _wheel_right.angular_vel) * _config->wheelRadius / 2.0;
  _heading += robot_angular_vel * time_delta;

  _x += _linear_vel * time_delta * std::cos(_heading);
  _y += _linear_vel * time_delta * std::sin(_heading);
}

void drivetrain_window::render(cv::Mat &img) {
  for (double m = 0; m <= 1 / scale; m += 1) {
    ui::line{ui::point{0, m * scale}, ui::point{1, m * scale}}.draw(img, ui::colour::gray() * 0.5, 1);
    ui::line{ui::point{m * scale, 0}, ui::point{m * scale, 1}}.draw(img, ui::colour::gray() * 0.5, 1);
  }

  ui::point{0.05, 0.10}.textl(img, ("X: " + ui::utils::fmt_precision(_x, 2) + "m").c_str(), 0.5, ui::colour::white());
  ui::point{0.35, 0.10}.textl(img, ("Y: " + ui::utils::fmt_precision(_y, 2) + "m").c_str(), 0.5, ui::colour::white());

  if (_enc_sim_left != nullptr && _enc_sim_right != nullptr) {
    ui::point{0.05, 0.15}.textl(img, ("Enc(L): " + std::to_string(_config->leftDrive.encoder->GetEncoderTicks())).c_str(), 0.5, ui::colour::white());
    ui::point{0.35, 0.15}.textl(img, ("Enc(R): " + std::to_string(_config->rightDrive.encoder->GetEncoderTicks())).c_str(), 0.5, ui::colour::white());
  }

  draw_robot(img);
}

void drivetrain_window::draw_robot(cv::Mat &img) {
  ui::point offset{0.15 + _x * scale, 0.5 - _y * scale};

  double width = _config->trackwidth;
  double depth = _config->trackdepth;

  ui::point fl{-width / 2, depth / 2}, fr{width / 2, depth / 2};
  ui::point bl{-width / 2, -depth / 2}, br{width / 2, -depth / 2};

  double rot = -3.14159265 / 2 - _heading;

  ui::line left = offset + ui::line{ fl.rotate(rot) * scale, bl.rotate(rot) * scale };
  ui::line right = offset + ui::line{ fr.rotate(rot) * scale, br.rotate(rot) * scale };
  ui::line front = offset + ui::line{ fl.rotate(rot) * scale, fr.rotate(rot) * scale };
  ui::line back = offset + ui::line{ bl.rotate(rot) * scale, br.rotate(rot) * scale };

  left.draw(img, _left_colour * (0.5 + std::abs(get_motor_val(true)) / 2), 2);
  right.draw(img, _right_colour * (0.5 + std::abs(get_motor_val(false)) / 2), 2);
  front.draw(img, _front_colour, 2);
  back.draw(img, ui::colour::gray(), 2);
}