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

	// A target for reverse iteration update
	auto bug_1st_target_r = new Bug;
	bug_1st_target_r->position = Point(max_corner - 600.0f, min_corner); // top right corner, the closest target initially

	auto bug_2nd_target = new Bug;
	bug_2nd_target->position = Point(min_corner, max_corner - 100.0f); // bottom left corner, a bit further from the hunter bug

	// A target for reverse iteration update
	auto bug_2nd_target_r = new Bug;
	bug_2nd_target_r->position = Point(min_corner, max_corner - 200.0f); // bottom left corner, a bit further from the hunter bug

	auto bug_hunter = new Bug; // very hungry
	bug_hunter->position = Point(min_corner, min_corner);

	game.AddObject(bug_1st_target);
	game.AddObject(bug_2nd_target);

	game.AddObject(bug_hunter);

	game.AddObject(bug_1st_target_r);
	game.AddObject(bug_2nd_target_r);

	game.OnBugsSpawned();
	
	BugBase* target = nullptr;
	game.onBugUpdate_Begin = [&target, bug_hunter](auto bug)
	{
		if (bug == bug_hunter)
			target = bug->FindBugToEat();
	};

	int frame_counter = 0;

	game.onBugUpdate_End = [bug_1st_target, bug_2nd_target, 
		bug_1st_target_r, bug_2nd_target_r, &frame_counter](auto bug)
	{
		// On the second frame target bugs do huge jumps across the map
		if (frame_counter != 1)
			return;

		if (bug == bug_1st_target)
			bug_1st_target->position = bug_1st_target->position + Point(0.0f, 4000.0f);

		if (bug == bug_1st_target_r)
			bug_1st_target_r->position = bug_1st_target_r->position + Point(0.0f, 4000.0f);

		if (bug == bug_2nd_target)
			bug_2nd_target->position = bug_2nd_target->position - Point(0.0f, 4000.0f);

		if (bug == bug_2nd_target_r)
			bug_2nd_target_r->position = bug_2nd_target_r->position - Point(0.0f, 4000.0f);
	};

	game.Update(1.0f); // 1st frame
	frame_counter = 1;
	// 1st target bug jumps away from the hunter
	// 2nd jumps closer to it
	game.Update(1.0f); // 2nd frame

	CHECK(target == bug_2nd_target || target == bug_2nd_target_r);
}

void RandomTest(std::default_random_engine& rng, uint num_bugs)
{
	Game game;

	for (uint i = 0; i < num_bugs; i++)
	{
		auto bug = new Bug;
		std::uniform_real_distribution<float> distr(min_corner, max_corner);

		bug->position = Point(distr(rng), distr(rng));
		game.AddObject(bug);
	}

	auto naive_find_target = [&game](BugBase* hunter)
	{
		Bug* target = nullptr;
		float min_dist = std::numeric_limits<float>::max();
		for (auto object : game.objects)
		{
			if (auto bug = dynamic_cast<Bug*>(object))
			{
				if (bug == hunter)
					continue;

				if (bug->disabled)
					continue;

				if (bug->id > hunter->id)
					continue;

				float dist = hunter->position.Distance(bug->position);
				if (dist < min_dist)
				{
					min_dist = dist;
					target = bug;
				}
			}
		}

		return target;
	};

	struct Prey
	{
		BugBase* expected = nullptr;
		BugBase* got = nullptr;
	};
	std::unordered_map<BugBase*, Prey> hunter2prey;

	game.onBugUpdate_Begin = [&naive_find_target, &hunter2prey](auto bug)
	{
		auto target_naive = naive_find_target(bug);
		auto target = bug->FindBugToEat();

		hunter2prey[bug] = { target_naive, target };
	};

	game.Update(1.0f);

	for (auto&& [hunter, prey] : hunter2prey)
	{
		CHECK(prey.expected == prey.got);
		if (prey.expected != prey.got)
		{
			std::string expected_str = prey.expected ? std::format("Bug#{}", prey.expected->id) : "nullptr";
			std::string got_str = prey.got ? std::format("Bug#{}", prey.got->id) : "nullptr";
			Log("For Bug#{}: expected {}, got {}", hunter->id, expected_str, got_str);
			break;
		}
	}
}

std::default_random_engine rng(0);

TEST(RandomTest_1)
{
	RandomTest(rng, 1024);
}

TEST(RandomTest_2)
{
	RandomTest(rng, 2048);
}

TEST(RandomTest_3)
{
	RandomTest(rng, 4096);
}

TEST(RandomTest_4)
{
	RandomTest(rng, 8192);
}