#include "pch.h"
#include "Game.h"
#include "Bug.h"

static const float min_corner = 100.0f; // Should be 0.0f but don't want to put things on the potential cell borders
static const float max_corner = 6300.0f; // Should be 6400.0f but don't want to put things on the potential cell borders

TEST(DiametralBugs)
{
	Game game;
	auto bug_target = new Bug;
	bug_target->position = Point(min_corner, min_corner);
	game.AddObject(bug_target);

	auto bug_hunter = new Bug;
	bug_hunter->position = Point(max_corner, max_corner);
	game.AddObject(bug_hunter);

	game.OnBugsSpawned();
	game.Update(1.0f);

	auto bug_to_eat = bug_hunter->FindBugToEat();

	CHECK(bug_to_eat == bug_target);
}

TEST(ChunkBordersCrossedOnUpdate)
{
	Game game;

	auto bug_1st_target = new Bug;
	bug_1st_target->position = Point(max_corner - 500.0f, min_corner); // top right corner, the closest target initially
	game.AddObject(bug_1st_target);

	auto bug_2nd_target = new Bug;
	bug_2nd_target->position = Point(min_corner, max_corner - 100.0f); // bottom left corner, a bit further from the hunter bug
	game.AddObject(bug_2nd_target);

	auto bug_hunter = new Bug; // very hungry
	bug_hunter->position = Point(min_corner, min_corner);
	game.AddObject(bug_hunter);

	game.OnBugsSpawned();
	
	BugBase* target = nullptr;
	game.onBugUpdate_Begin = [&target, bug_hunter](auto bug)
	{
		if (bug == bug_hunter)
			target = bug->FindBugToEat();
	};

	int frame_counter = 0;

	game.onBugUpdate_End = [bug_1st_target, bug_2nd_target, &frame_counter](auto bug)
	{
		// On the second frame target bugs do huge jumps across the map
		if (frame_counter != 1)
			return;

		if (bug == bug_1st_target)
			bug_1st_target->position = bug_1st_target->position + Point(0.0f, 4000.0f);

		if (bug == bug_2nd_target)
			bug_2nd_target->position = bug_2nd_target->position - Point(0.0f, 4000.0f);
	};

	game.Update(1.0f); // 1st frame
	frame_counter = 1;
	// 1st target bug jumps away from the hunter
	// 2nd jumps closer to it
	game.Update(1.0f); // 2nd frame

	CHECK(target == bug_2nd_target);
}