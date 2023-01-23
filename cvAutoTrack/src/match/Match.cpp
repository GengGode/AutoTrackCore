#include "pch.h"
#include "Match.h"
#include "resources/Resources.h"
#include "surf/SurfMatch.h"

// #include <filesystem>

void TianLi::Match::get_avatar_position(const GenshinMinimap& genshin_minimap, GenshinAvatarPosition& out_genshin_position)
{
	static SurfMatch surf_match;
	static bool is_init = false;
	if (!is_init && false)
	{
		std::vector<cv::KeyPoint> gi_map_keypoints;
		cv::Mat gi_map_descriptors;
		
		surf_match.setMap(Resources::getInstance().MapTemplate);
		
		get_map_keypoint(gi_map_keypoints, gi_map_descriptors);
		surf_match.Init(gi_map_keypoints, gi_map_descriptors);

		is_init = true;
	}
	if (genshin_minimap.is_run_init_start == true)
	{
		if (is_init)return;
		Resources::getInstance().install();
		
		std::vector<cv::KeyPoint> gi_map_keypoints;
		cv::Mat gi_map_descriptors;
		

		surf_match.setMap(Resources::getInstance().MapTemplate);

		//surf_match.detector = cv::xfeatures2d::SURF::create(surf_match.minHessian);
		//surf_match.detectorSomeMap = cv::xfeatures2d::SURF::create(surf_match.minHessian);

		get_map_keypoint(gi_map_keypoints, gi_map_descriptors);
		surf_match.Init(gi_map_keypoints, gi_map_descriptors);

		is_init = true;
		return;
	}

	if (genshin_minimap.is_run_uninit_start == true)
	{
		surf_match.UnInit();
		Resources::getInstance().release();
		is_init = false;
		return;
	}

	if (genshin_minimap.config.is_find_paimon == false)
	{
		return;
	}

	if (genshin_minimap.img_minimap.empty())
	{
		return;
	}

	surf_match.setMiniMap(genshin_minimap.img_minimap);

	surf_match.match();

	out_genshin_position.position = surf_match.getLocalPos();
	if (surf_match.is_success_match)
	{
		out_genshin_position.config.img_last_match_minimap = genshin_minimap.img_minimap.clone();
		out_genshin_position.config.is_exist_last_match_minimap = true;
	}
	else
	{
		out_genshin_position.config.is_exist_last_match_minimap = false;
	}
}