/// @file pathest/kalman_filter.cc
/// @brief Class for Kalman filter.
/// @bug Hardcoded matrices.
//===----------------------------------------------------------------------===//

#include "pathest/kalman_filter.h"

#include <armadillo>

#include "pathest/location.h"

namespace pathest {

arma::mat initA() {
  arma::mat A = arma::eye(4, 4);
  A(0, 2) = A(1, 3) = 0.2;
  return A;
}

arma::mat initH() {
  arma::mat H = arma::zeros(4, 4);
  H(0, 0) = H(0, 2) = H(1, 1) = H(1, 3) = 1.0;
  return H;
}

arma::mat initQ() {
  arma::mat Q = arma::zeros(4, 4);
  Q(2, 2) = Q(3, 3) = 0.1;
  return Q;
}

const arma::mat KalmanFilter::A_ = initA();
const arma::mat KalmanFilter::H_ = initH();
const arma::mat KalmanFilter::I_ = arma::eye(4, 4);
const arma::mat KalmanFilter::Q_ = initQ();
const arma::mat KalmanFilter::R_ = 0.1 * arma::eye(4, 4);

KalmanFilter::KalmanFilter() :
  m_(arma::zeros(4, 1)),
  x_(arma::zeros(4, 1)),
  y_(arma::zeros(4, 1)),
  K_(arma::zeros(4, 4)),
  P_(arma::zeros(4, 4)),
  S_(arma::zeros(4, 4)) {}

Location KalmanFilter::predict(const Location &loc) {
  this->m_(0, 0) = loc.x();
  this->m_(1, 0) = loc.y();

  // Prediction step.
  this->x_ = this->A_ * this->x_;
  this->P_ = (this->A_ * this->P_ * this->A_.t()) + this->Q_;

  // Update step.
  this->S_ = (this->H_ * this->P_ * this->H_.t()) + this->R_;
  this->K_ = this->P_ * this->H_.t() * solve(this->S_, this->I_);
  this->y_ = this->m_ - (this->H_ * this->x_);
  this->x_ = this->x_ + (this->K_ * this->y_);
  this->P_ = (this->I_ - (this->K_ * this->H_)) * this->P_;

  double pred_x = this->x_(0, 0);
  double pred_y = this->x_(1, 0);
  return Location(pred_x, pred_y, loc.t());
}

}  // namespace pathest
