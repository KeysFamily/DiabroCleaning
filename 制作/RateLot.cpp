#include "RateLot.h"

RateLot::RateLot(std::vector<float> dist_)
	:dist(dist_.begin(), dist_.end())
{
	std::random_device seed_gen;
	engine.seed(seed_gen());
}

RateLot::~RateLot() {
	probabilities.clear();
	dist.reset();
}

std::size_t RateLot::result() {
	return dist(engine);
}
