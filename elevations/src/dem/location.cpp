﻿#include "location.h"

using elevations::dem::location;

location::location(double x, double y, ptr<elevations::producer::height_layer> height_layer)
	: Object("Location")
	, x_(x)
	, y_(y)
	, level_(0)
	, tx_(0)
	, ty_(0)
	, height_layer_(height_layer)
{
	assert(height_layer_ != nullptr);
	set_level(height_layer_->get_min_level());
}

double location::get_height() const
{
	return height_layer_->get_height(level_, x_, y_);
}

proland::TileCache::Tile* location::get_tile(unsigned deadline) const
{
	return height_layer_->get_tile(level_, tx_, ty_, deadline);
}

void location::put_tile() const
{
	height_layer_->put_tile(height_layer_->find_tile(level_, tx_, ty_));
}

size_t location::set_level(size_t level)
{
	assert(height_layer_->get_min_level() <= level && level <= height_layer_->get_max_level());

	auto result = level_;
	level_ = level;

	auto quad_size = height_layer_->getRootQuadSize();
	tx_ = details::physical_to_logical(x_, quad_size, level);
	ty_ = details::physical_to_logical(y_, quad_size, level);

	return result;
}

void location::schedule(ptr<TaskGraph> task_graph) const
{
	height_layer_->schedule(task_graph);
}

int elevations::dem::details::physical_to_logical(double value, double quad_size, int level)
{
	auto result = value / quad_size + 0.5f;
	return result >= 0.0f && result <= 1.0f ? result * (1 << level) : -1;
}