/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * The berserker.
 *
 * =======================================================================
 */

#include "../../header/local.h"
#include "tower_spawner.h"

static int sound_pain;
static int sound_die;
static int sound_idle;
static int sound_punch;
static int sound_sight;
static int sound_search;

static int  sound_step;
static int  sound_step2;

void
tower_spawner_footstep(edict_t *self)
{
	if (!g_monsterfootsteps->value)
		return;

	// Lazy loading for savegame compatibility.
	if (sound_step == 0 || sound_step2 == 0)
	{
		sound_step = gi.soundindex("berserk/step1.wav");
		sound_step2 = gi.soundindex("berserk/step2.wav");
	}

	if (randk() % 2 == 0)
	{
		gi.sound(self, CHAN_BODY, sound_step, 1, ATTN_NORM, 0);
	}
	else
	{
		gi.sound(self, CHAN_BODY, sound_step2, 1, ATTN_NORM, 0);
	}
}


void
tower_spawner_sight(edict_t *self, edict_t *other /* unused */)
{
	if (!self)
	{
		return;
	}

	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

void
tower_spawner_search(edict_t *self)
{
	if (!self)
	{
		return;
	}

	gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

void tower_spawner_fidget(edict_t *self);

static mframe_t tower_spawner_frames_stand[] = {
	{ai_stand, 0, tower_spawner_fidget},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL}
};

mmove_t tower_spawner_move_stand =
{
	FRAME_stand1,
	FRAME_stand5,
	tower_spawner_frames_stand,
	NULL};

void
tower_spawner_stand(edict_t *self)
{
	if (!self)
	{
		return;
	}

	self->monsterinfo.currentmove = &tower_spawner_move_stand;
}

static mframe_t tower_spawner_frames_stand_fidget[] = {
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL},
	{ai_stand, 0, NULL}
};

mmove_t tower_spawner_move_stand_fidget =
{
	FRAME_standb1,
	FRAME_standb20,
	tower_spawner_frames_stand_fidget,
	tower_spawner_stand
};

void
tower_spawner_fidget(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if (self->enemy)
	{
		return;
	}

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		return;
	}

	if (random() > 0.15)
	{
		return;
	}

	self->monsterinfo.currentmove = &tower_spawner_move_stand_fidget;
	gi.sound(self, CHAN_WEAPON, sound_idle, 1, ATTN_IDLE, 0);
}

static mframe_t tower_spawner_frames_walk[] = {
	{ai_walk, 9.1, NULL},
	{ai_walk, 6.3, NULL},
	{ai_walk, 4.9, NULL},
	{ai_walk, 6.7, tower_spawner_footstep},
	{ai_walk, 6.0, NULL},
	{ai_walk, 8.2, NULL},
	{ai_walk, 7.2, NULL},
	{ai_walk, 6.1, NULL},
	{ai_walk, 4.9, tower_spawner_footstep},
	{ai_walk, 4.7, NULL},
	{ai_walk, 4.7, NULL},
	{ai_walk, 4.8, NULL}
};

mmove_t tower_spawner_move_walk =
{
	FRAME_walkc1,
	FRAME_walkc11,
	tower_spawner_frames_walk,
	NULL
};

void
tower_spawner_walk(edict_t *self)
{
	if (!self)
	{
		return;
	}

	self->monsterinfo.currentmove = &tower_spawner_move_walk;
}

static mframe_t tower_spawner_frames_run1[] = {
	{ai_run, 21, NULL},
	{ai_run, 11, tower_spawner_footstep},
	{ai_run, 21, NULL},
	{ai_run, 25, NULL},
	{ai_run, 18, tower_spawner_footstep},
	{ai_run, 19, NULL}
};

mmove_t tower_spawner_move_run1 =
{
	FRAME_run1,
	FRAME_run6,
	tower_spawner_frames_run1,
	NULL
};

void
tower_spawner_run(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		self->monsterinfo.currentmove = &tower_spawner_move_stand;
	}
	else
	{
		self->monsterinfo.currentmove = &tower_spawner_move_run1;
	}
}

void
tower_spawner_attack_spike(edict_t *self)
{
	static vec3_t aim = {MELEE_DISTANCE, 0, -24};

	if (!self)
	{
		return;
	}

	fire_hit(self, aim, (15 + (randk() % 6)), 400); /*	Faster attack -- upwards and backwards */
}

void
tower_spawner_swing(edict_t *self)
{
	if (!self)
	{
		return;
	}

	gi.sound(self, CHAN_WEAPON, sound_punch, 1, ATTN_NORM, 0);
}

static mframe_t tower_spawner_frames_attack_spike[] = {
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL},
	{ai_charge, 0, tower_spawner_swing},
	{ai_charge, 0, tower_spawner_attack_spike},
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL}
};

mmove_t tower_spawner_move_attack_spike =
{
	FRAME_att_c1,
	FRAME_att_c8,
	tower_spawner_frames_attack_spike,
	tower_spawner_run
};

void
tower_spawner_attack_club(edict_t *self)
{
	vec3_t aim;

	if (!self)
	{
		return;
	}

	VectorSet(aim, MELEE_DISTANCE, self->mins[0], -4);
	fire_hit(self, aim, (5 + (randk() % 6)), 400);       /* Slower attack */
}

static mframe_t tower_spawner_frames_attack_club[] = {
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL},
	{ai_charge, 0, tower_spawner_footstep},
	{ai_charge, 0, NULL},
	{ai_charge, 0, tower_spawner_swing},
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL},
	{ai_charge, 0, tower_spawner_attack_club},
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL},
	{ai_charge, 0, NULL}
};

mmove_t tower_spawner_move_attack_club =
{
	FRAME_att_c9,
	FRAME_att_c20,
	tower_spawner_frames_attack_club,
   	tower_spawner_run
};

void
tower_spawner_strike(edict_t *self)
{
	/* Unused, but removal is
	   very PITA. Let it be... */
}

static mframe_t tower_spawner_frames_attack_strike[] = {
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, tower_spawner_footstep},
	{ai_move, 0, tower_spawner_swing},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, tower_spawner_strike},
	{ai_move, 0, tower_spawner_footstep},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 9.7, NULL},
	{ai_move, 13.6, tower_spawner_footstep}
};

mmove_t tower_spawner_move_attack_strike =
{
	FRAME_att_c21,
   	FRAME_att_c34,
   	tower_spawner_frames_attack_strike,
	tower_spawner_run
};

void
tower_spawner_melee(edict_t *self)
{
	if (!self)
	{
		return;
	}

	if ((randk() % 2) == 0)
	{
		self->monsterinfo.currentmove = &tower_spawner_move_attack_spike;
	}
	else
	{
		self->monsterinfo.currentmove = &tower_spawner_move_attack_club;
	}
}

static mframe_t tower_spawner_frames_pain1[] = {
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL}
};

mmove_t tower_spawner_move_pain1 =
{
	FRAME_painc1,
	FRAME_painc4,
	tower_spawner_frames_pain1,
	tower_spawner_run};

static mframe_t tower_spawner_frames_pain2[] = {
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL}
};

mmove_t tower_spawner_move_pain2 =
{
	FRAME_painb1,
	FRAME_painb20,
	tower_spawner_frames_pain2,
	tower_spawner_run
};

void
tower_spawner_pain(edict_t *self, edict_t *other /* unused */,
		float kick /* unused */, int damage)
{
	if (!self)
	{
		return;
	}

	if (self->health < (self->max_health / 2))
	{
		self->s.skinnum = 1;
	}

	if (level.time < self->pain_debounce_time)
	{
		return;
	}

	self->pain_debounce_time = level.time + 3;
	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (skill->value == SKILL_HARDPLUS)
	{
		return; /* no pain anims in nightmare */
	}

	if ((damage < 20) || (random() < 0.5))
	{
		self->monsterinfo.currentmove = &tower_spawner_move_pain1;
	}
	else
	{
		self->monsterinfo.currentmove = &tower_spawner_move_pain2;
	}
}

void
tower_spawner_dead(edict_t *self)
{
	if (!self)
	{
		return;
	}

	VectorSet(self->mins, -16, -16, -24);
	VectorSet(self->maxs, 16, 16, -8);
	self->movetype = MOVETYPE_TOSS;
	self->svflags |= SVF_DEADMONSTER;
	self->nextthink = 0;
	gi.linkentity(self);
}

static mframe_t tower_spawner_frames_death1[] = {
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL}
};

mmove_t tower_spawner_move_death1 =
{
	FRAME_death1,
	FRAME_death13,
	tower_spawner_frames_death1,
	tower_spawner_dead
};

static mframe_t tower_spawner_frames_death2[] = {
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL},
	{ai_move, 0, NULL}
};

mmove_t tower_spawner_move_death2 =
{
	FRAME_deathc1,
   	FRAME_deathc8,
	tower_spawner_frames_death2,
	tower_spawner_dead};

void
tower_spawner_die(edict_t *self, edict_t *inflictor /* unused */, edict_t *attacker /* unused */,
		int damage, vec3_t point /* unused */)
{
	int n;

	if (!self)
	{
		return;
	}

	if (self->health <= self->gib_health)
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex( "misc/udeath.wav"), 1, ATTN_NORM, 0);

		for (n = 0; n < 2; n++)
		{
			ThrowGib(self, "models/objects/gibs/bone/tris.md2",
					damage, GIB_ORGANIC);
		}

		for (n = 0; n < 4; n++)
		{
			ThrowGib(self, "models/objects/gibs/sm_meat/tris.md2",
					damage, GIB_ORGANIC);
		}

		ThrowHead(self, "models/objects/gibs/head2/tris.md2",
				damage, GIB_ORGANIC);
		self->deadflag = DEAD_DEAD;
		return;
	}

	if (self->deadflag == DEAD_DEAD)
	{
		return;
	}

	gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
	self->deadflag = DEAD_DEAD;
	self->takedamage = DAMAGE_YES;

	if (damage >= 50)
	{
		self->monsterinfo.currentmove = &tower_spawner_move_death1;
	}
	else
	{
		self->monsterinfo.currentmove = &tower_spawner_move_death2;
	}
}

qboolean tower_spawner_upgrade(edict_t *self, edict_t *upgrader)
{
	if (upgrader->client)
	{
		gi.centerprintf(upgrader, "Upgrade attempted!");
	}
}

/*
 * QUAKED monster_tower_spawner (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
 */
void
SP_monster_tower_spawner(edict_t *self)
{
	if (!self)
	{
		return;
	}

	// Force recaching at next footstep to ensure
	// that the sound indices are correct.
	sound_step = 0;
	sound_step2 = 0;

	/* pre-caches */
	sound_pain = gi.soundindex("berserk/berpain2.wav");
	sound_die = gi.soundindex("berserk/berdeth2.wav");
	sound_idle = gi.soundindex("berserk/beridle1.wav");
	sound_punch = gi.soundindex("berserk/attack.wav");
	sound_search = gi.soundindex("berserk/bersrch1.wav");
	sound_sight = gi.soundindex("berserk/sight.wav");

	self->s.modelindex = gi.modelindex("models/monsters/berserk/tris.md2");
	VectorSet(self->mins, -16, -16, -24);
	VectorSet(self->maxs, 16, 16, 32);
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 240;
	self->gib_health = -60;
	self->mass = 250;

	self->pain = tower_spawner_pain;
	self->die = tower_spawner_die;

	self->monsterinfo.stand = tower_spawner_stand;
	self->monsterinfo.walk = tower_spawner_walk;
	self->monsterinfo.run = tower_spawner_run;
	self->monsterinfo.dodge = NULL;
	self->monsterinfo.attack = NULL;
	self->monsterinfo.melee = tower_spawner_melee;
	self->monsterinfo.sight = tower_spawner_sight;
	self->monsterinfo.search = tower_spawner_search;

	self->monsterinfo.upgrade = tower_spawner_upgrade;

	self->monsterinfo.currentmove = &tower_spawner_move_stand;
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.aiflags |= AI_DEFENDER;
	self->monsterinfo.aiflags |= AI_GOOD_GUY;
	self->flags |= FL_NO_KNOCKBACK;

	gi.linkentity(self);

	walkmonster_start(self);
}
