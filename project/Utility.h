#pragma once
// ---------------------------------------------
// Research project for Gameplay Programming DAE (WIP)
// ---------------------------------------------
// Reference: https://mcguirev10.com/2019/01/03/ai-decision-making-with-utility-scores-part-1.html
// Reference: https://www.youtube.com/watch?v=M0Sx_M61ILU
// Reference: https://www.gdcvault.com/play/1012410/Improving-AI-Decision-Modeling-Through
// Reference: https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc10/slides/MarkDill_ImprovingAIUtilityTheory.pdf
// ---------------------------------------------

#include "EBlackboard.h"
#include "IExamInterface.h"

class Utility
{
	public:
		enum class CurveType
		{
			NONE,
			LINEAR,
			INVERSE_LINEAR,
			EXPONENTIAL,
			SINE,
			COSINE,
			LOGISTIC,
			LOGIT,
			SMOOTHSTEP,
			SMOOTHERSTEP
		};
		enum class ActionType
		{
			WANDER,
			FLEE_OUTSIDE,
			FLEE_INSIDE,
			FLEE_PURGE,
			FLEE_BITTEN,
			ATTACK,
			HOUSE_ENTER,
			HOUSE_CHECK,
			HOUSE_ITEMS,
			HOUSE_EXIT
		};

		Utility();
		~Utility() = default;

		Utility(const Utility&) = delete;
		Utility(Utility&&) noexcept = delete;
		Utility& operator=(const Utility&) = delete;
		Utility& operator=(Utility&&) noexcept = delete;

		void Initialize(Elite::Blackboard* pBlackboard);

		BehaviorType Reasoner();

	private:
		Elite::Blackboard* m_pBlackboard;
		float m_PI;
		double m_E;

		float Curve_Scoring(const CurveType& curveType, const float value);
		float ChooseAction(std::vector<float>& weights);
		float Considerations(const ActionType& actionType);

		float Remap(const float value, const float min_Old, const float max_Old, const float min_New, const float max_New);
		float Log(float base, float value) const { return log2(base) / log2(value); }

		Elite::Vector2 CalculateDirectionAwayFromTarget(AgentInfo& agent, std::vector<EntityInfo>* pEntitiesInFOV, float& distance);
		void SetUpData(const BehaviorType& behaviorType);
};

