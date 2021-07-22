#include <iostream>

#define M_PI 3.1415

namespace ParticleSimulation {
#define NUM_PARTICLES 10000

	typedef struct {
		float x;
		float y;
	} Vector2;

	typedef struct {
		Vector2 position;
		Vector2 velocity;
		float mass;
	} Particle;

	Particle particles[NUM_PARTICLES];

	float random_uniform(int max) { return 0; }

	void sleep(float dt) {}

	void InitializeParticles() {
		for (int i = 0; i < NUM_PARTICLES; ++i) {
			particles[i].position = Vector2{random_uniform(50), random_uniform(50)};
			particles[i].velocity = Vector2{0, 0};
			particles[i].mass = 1;
		}
	}

	Vector2 ComputeForce(Particle *particle) {
		return Vector2{0, (float) (particle->mass * -9.81)};
	}

	void RunSimulation() {
		float totalSimulationTime = 10;
		float currentTime = 0;
		float dt = 1;

		InitializeParticles();

		while (currentTime < totalSimulationTime) {
			sleep(dt);

			for (int i = 0; i < NUM_PARTICLES; ++i) {
				Particle *particle = &particles[i];
				Vector2 force = ComputeForce(particle);
				Vector2 acceleration = Vector2{force.x / particle->mass, force.y / particle->mass};
				particle->velocity.x += acceleration.x * dt;
				particle->velocity.y += acceleration.y * dt;
				particle->position.x += particle->velocity.x * dt;
				particle->position.y += particle->velocity.y * dt;
			}

			currentTime += dt;
		}
	}
}


namespace RigidBodySimulation {
	typedef struct {
		float x;
		float y;
	} Vector2;

	typedef struct {
		float width;
		float height;
		float mass;
		float momentOfInertia;
	} BoxShape;

	void CalculateBoxInertia(BoxShape *boxShape) {
		float m = boxShape->mass;
		float w = boxShape->width;
		float h = boxShape->height;
		boxShape->momentOfInertia = m * (w * w + h * h) / 12;
	}

	typedef struct {
		Vector2 position;
		Vector2 linearVelocity;
		Vector2 force;

		float angle;
		float angularVelocity;
		float torque;

		BoxShape shape;

	} RigidBody;


}
