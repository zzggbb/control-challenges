#include "turret.h"
#include "solutions/turret/moving_goal.h"
#include "visualization.h"
#include <iostream>

int main() {
  TurretVisualization visualizer;
  TurretState current_state{10 * deg, 0 * deg, -100 * deg / s};
  TurretController controller;
  sf::Clock clock;

  Time timer = 0;

  //Constants
  const Time dt = .005 * s;
  const auto kTheta = (1 * N) * (s/(m*kg)); 
  const Mass turret_mass = 30 * kg;
  const Length turret_radius = .5 * m;

  // 775 pro stats
  const Current stall_current = 134 * A, free_current = .7 * A;
  const Torque stall_torque = .71 * N * m;
  const AngularVelocity free_speed = 18730 * rpm;

  const Resistance R = 12 * V / stall_current;
  const auto kT = stall_torque / stall_current;
  const auto kV = (12 * V - free_current * R) / free_speed;
  const Unitless G = (1/2.7);

  const auto J = (turret_radius * turret_radius) * turret_mass;

  while (visualizer.is_open()) {
    if (!visualizer.is_complete()) {

      //Calculate forces applied to system
      AngularAcceleration turret_friction =  (-kTheta * current_state.angular_velocity); 
      Voltage u = controller.Update(current_state, dt);
      Torque torque =
          kT * u / (G * R) - kT * kV * current_state.angular_velocity / (G * G * R);
      AngularAcceleration turret_acceleration = (torque/J) + turret_friction;

      //Move goal
      if(timer < 10 *s) {
      current_state.goal = std::cos(std::sin(timer.to(s)/4));
      } else if (timer < 20 * s) {
      current_state.goal = std::cos(timer.to(s)/4);
      }


      current_state.angular_velocity += turret_acceleration * dt;
      current_state.angle += current_state.angular_velocity * dt;

      visualizer.SetState(current_state);

      if(std::abs((current_state.angle - 70 * deg).to(deg)) < 0.1 &&
         std::abs((current_state.angular_velocity.to(deg/s))) < 0.1) {
        visualizer.Complete();
      }

      //TODO (Anyone) I don't know how I should do the completion function for this (checking if the goal is being accurately followed)

      timer += dt;
    }
    visualizer.Render();

    sf::sleep(sf::seconds(dt()) - clock.restart());
  }
}
