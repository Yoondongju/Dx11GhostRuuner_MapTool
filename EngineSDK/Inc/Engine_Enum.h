#pragma once

namespace Engine
{
	enum KEY
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		Q,
		W,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,


		A,
		S,
		D,
		F,
		G,
		Z,
		X,
		C,
		V,
		B,

		ALT,
		CTRL,
		LSHIFT,
		SPACE,
		ENTER,
		ESC,

		LBUTTON,
		RBUTTON,
		ONE,

		LAST,
	};

	enum KEY_STATE
	{
		NONE, // 눌리지않고 이전에도 안눌림
		TAP,  // 막 누른시점
		HOLD, // 누르고 있는
		AWAY, // 막 뗀 시점

	};

	enum UI_TYPE
	{
		UITYPE_NONE,
		HPMP_PANEL,
		HP,
		MP,
		EXP,
		LEVEL,
		DIE,
		BOSS_HP,
		QUICK_SLOT,
		CONVERSATION,
		INVENTORY,
		EQUIPMENT,
		STORE,
		STAT,
		SKILL,
		SKILL_ICON,
		DAMAGE_EFFECT,
		BUTTON,


		NOGOLD,
		UI_END
	};


	enum GROUP_TYPE
	{
		DEFAULT,
		PLAYER,
		MONSTER,
		PROJ_PLAYER,
		PROJ_MONSTER,

		END = 32
	};


	enum COMPONENT_TYPE
	{
		ANIMATOR,
		COLLIDER_CUBE,
		COLLIDER_RECT,
		FSM,
		TEXTURE,
		TRANSFORM,
		VIBUFFER_CUBE,
		VIBUFFER_RECT,
		VIBUFFER_TERRAIN,

		COMPONENT_END
	};

}