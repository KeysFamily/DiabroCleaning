#pragma once
#include <random>
#include <vector>

/// <summary>
/// 概　　　要：設定した抽選確率をもとに抽選結果を返す
/// 作　成　者：長谷川勇一朗
/// 作成年月日：2023/6/28
/// </summary>

class RateLot
{
	std::vector<float> probabilities;
	std::discrete_distribution<std::size_t> dist;
	std::mt19937 engine;
public:
	/// <summary>
	/// 確率を決める
	/// 抽選確率の割合をリスト化して渡す
	/// </summary>
	/// <param name="dist_">抽選比率のリスト</param>
	RateLot(std::vector<float> dist_);

	~RateLot();

	/// <summary>
	/// 設定した確率に基づいて抽選結果(インデックス)を返す
	/// </summary>
	/// <returns>0～コンストラクタ引数で渡した配列の要素数-1のいずれかの抽選結果</returns>
	std::size_t result();
};
