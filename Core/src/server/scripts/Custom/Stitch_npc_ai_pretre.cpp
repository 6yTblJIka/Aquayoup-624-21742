////#########################################################################################################################################################################################################################################
// Copyright (C) Juin 2020 Stitch pour Aquayoup
// AI generique npc par classe : PRETRE V1.0
// Il est possible d'influencer le temp entre 2 cast avec `BaseAttackTime` & `RangeAttackTime` 
// Necessite dans Creature_Template :
// Minimun  : UPDATE `creature_template` SET `ScriptName` = 'Stitch_npc_ai_pretre',`AIName` = '' WHERE (entry = 15100003);
// Optionel : UPDATE `creature_template` SET `HealthModifier` = 2, `ManaModifier` = 3, `ArmorModifier` = 1, `DamageModifier` = 2,`BaseAttackTime` = 2000, `RangeAttackTime` = 2000 WHERE(entry = 15100003);
// Optionel : Utilisez pickpocketloot de creature_template pour passer certains parametres (Solution choisit afin de rester compatible avec tout les cores). Si pickpocketloot = 1 (branche1 forc�), pickpocketloot = 2 (branche2 forc�), etc
//###########################################################################################################################################################################################################################################
// # npc de Test Stitch_npc_ai_pretre  .npc 15100003
// REPLACE INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `femaleName`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_unk`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `HealthModifier`, `HealthModifierExtra`, `ManaModifier`, `ManaModifierExtra`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
// (15100003, 0, 0, 0, 0, 0, 7669, 0, 0, 0, 'npc_ai_Pretre', '', '', '', 0, 90, 90, 0, 0, 2102, 0, 1.01, 1.01, 0.7, 0, 0, 2000, 2000, 1, 1, 2, 0, 2048, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 2, 1, 3, 1, 2, 2, 1, 0, 144, 1, 0, 0, 'Stitch_npc_ai_pretre', 20173);
// REPLACE INTO `creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES
// (15100003, 1, 15979, 0, 0, 0),
// (15100003, 2, 15979, 0, 0, 0),
// (15100003, 3, 15979, 0, 0, 0);
//###########################################################################################################################################################################################################################################



//################################################################################################
//StitchAI AI Pretre
//################################################################################################

class Stitch_npc_ai_pretre : public CreatureScript
{
public: Stitch_npc_ai_pretre() : CreatureScript("Stitch_npc_ai_pretre") { }

		struct Stitch_npc_ai_pretreAI : public ScriptedAI
		{
			Stitch_npc_ai_pretreAI(Creature* creature) : ScriptedAI(creature) { }

			uint32 BrancheSpe = 1;													// Choix de la Sp�cialisation : Ombre=1, Discipline=2
			uint32 NbrDeSpe = 2;													// Nombre de Sp�cialisations
			uint32 ForceBranche;
			uint32 Random;
			uint32 DistanceDeCast = 30;												// Distance max a laquelle un npc attaquera , au dela il quite le combat
			uint32 ResteADistance = 10;												// Distance max a laquelle un npc s'approchera
			uint32 Dist;															// Distance entre le npc et sa cible
			uint32 Mana;
			uint32 MaxMana = me->GetMaxPower(POWER_MANA);

			Unit* victim = me->GetVictim();

			// Definitions des variables Cooldown et le 1er lancement
			uint32 Cooldown_Spell1 = 2500;
			uint32 Cooldown_Spell2 = 1500;
			uint32 Cooldown_Spell3 = 500;
			uint32 Cooldown_Spell_Heal = 3000;										// Cooldown pour la fr�quence du heal
			uint32 Cooldown_RegenMana = 3000;										// Cooldown pour le regen du mana
			uint32 Cooldown_ResteADistance = 4000;									// Test si en contact pour s'eloigner
			uint32 Cooldown_Spell_Bouclier = 4000;									// Cooldown pour mot de pouvoir : bouclier
			uint32 Cooldown_Npc_Emotes = urand(5000, 8000);

			// Spells Divers
			uint32 Buf_all = 21562;													// Mot de pouvoir�: Robustesse 21562
			uint32 Buf_branche1 = 15473;											// Forme d'Ombre 15473
			uint32 Buf_branche2 = 81700;											// Archange 81700 (soin +20% 15s)
			uint32 Spell_Heal_Caster = 8004;  										// R�novation 139 (rend pv a l'instant + 12s)
			uint32 Spell_Heal_Heal = 139;  											// R�novation 139 (rend pv a l'instant + 12s)
			uint32 Etreinte_Vampirique = 15286;										// Etreinte vampirique
			uint32 Nova_Sacree = 132157;											// Nova sacr�e dmg/heal 12m
			uint32 Mot_de_pouvoir_Bouclier = 17;									// Mot de pouvoir : Bouclier
			uint32 Soins_Rapides = 2061;											// Soins Rapides

			// Spells Ombre
			uint32 Spell_branche1_agro;
			uint32 Spell_branche1_1;
			uint32 Spell_branche1_2;
			uint32 Spell_branche1_3;
			uint32 branche1_agro[4] = { 15487, 15487, 34433, 64044 };				// Ombrefiel 34433, Horreur psychique 64044 (fear 5s), Silence 15487 5s
			uint32 branche1_1[4] = { 15407, 15407, 15407, 48045 };					// Fouet mental 15407, Incandescence mentale 48045
			uint32 branche1_2[2] = { 8092, 73510 };									// Attaque mentale 8092, Pointe mentale 73510
			uint32 branche1_3[3] = { 34914, 2944, 589 };							// Toucher vampirique 34914,  Peste d�vorante 2944, Mot de l�ombre:Douleur 589

			// Spells Discipline
			uint32 Spell_branche2_agro;	//    
			uint32 Spell_branche2_1;
			uint32 Spell_branche2_2;
			uint32 Spell_branche2_3;
			uint32 branche2_agro[6] = { 15487, 15487, 15487, 88625, 88625, 64044 };	// Horreur psychique 64044 (fear 5s), Silence 15487 5s , Mot sacr�: Ch�tier 88625 (stun 3s) , 
			uint32 branche2_1[2] = { 585, 585 };									// Ch�timent 585  
			uint32 branche2_2[3] = { 14914, 14914 ,14914 };							// Flammes sacr�es 14914, P�nitence 47540 dmg/heal
			uint32 branche2_3[3] = { 589, 589, 33206};								// Mot de l�ombre:Douleur 589, Suppression de la douleur 33206 degat-40% 8s

																					// Emotes
			uint32 Npc_Emotes[22] = { 1,3,7,11,15,16,19,21,22,23,24,53,66,71,70,153,254,274,381,401,462,482 };

			void JustRespawned() override
			{
				me->GetMotionMaster()->MoveTargetedHome();								// Retour home pour rafraichir client
				me->SetSpeedRate(MOVE_RUN, 1.01f);
				me->SetReactState(REACT_AGGRESSIVE);
			}
			void EnterCombat(Unit* /*who*/) override
			{
				if (!UpdateVictim() )
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);
				me->SetReactState(REACT_AGGRESSIVE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

				// Forcer le choix de la Sp�cialisation par creature_template->pickpocketloot
				ForceBranche = me->GetCreatureTemplate()->pickpocketLootId;							// creature_template->pickpocketloot
				if (ForceBranche == 1) { BrancheSpe = 1; }											// branche1 forc�
				else if (ForceBranche == 2) { BrancheSpe = 2; }										// branche2 forc�
				else
				{
					// Sinon Choix de la Sp�cialisation Al�atoire
					BrancheSpe = urand(1, NbrDeSpe + 1);
				}

				if ((BrancheSpe > NbrDeSpe) || (BrancheSpe == 0)) { BrancheSpe = 2; }				// plus de chance d'etre Discipline


				// Spell a lancer a l'agro ------------------------------------------------------------------------------------------------------------------------
				me->CastSpell(me, Buf_all, true);													// Buf1 sur lui meme pour toutes les Sp�cialit�es

				switch (BrancheSpe)
				{
				case 1: // Si Ombre  ------------------------------------------------------------------------------------------------------------------------------
					me->CastSpell(me, Etreinte_Vampirique, true);									// Etreinte vampirique 15286

					me->CastSpell(me, Buf_branche1, true);											// Buf1 sur lui meme
					me->LoadEquipment(1, true);														// creature_equip_template 1

					// Tirages al�atoires des spells 
					Spell_branche1_agro = branche1_agro[urand(0, 3)];
					Spell_branche1_1 = branche1_1[urand(0, 3)];
					Spell_branche1_2 = branche1_2[urand(0, 1)];
					Spell_branche1_3 = branche1_3[urand(0, 2)];

					Random = urand(1, 3);
					if (Random == 1 && UpdateVictim()) { DoCastVictim(Spell_branche1_agro); }		// 1/3 Chance de lancer le sort d'agro
					break;

				case 2: // Si Discipline  -------------------------------------------------------------------------------------------------------------------------
					me->CastSpell(me, Buf_branche2, true);											// Buf2 sur lui meme
					me->LoadEquipment(2, true);														// creature_equip_template 2

					// Tirages al�atoires des spells 
					Spell_branche2_agro = branche2_agro[urand(0, 5)];
					Spell_branche2_1 = branche2_1[urand(0, 1)];
					Spell_branche2_2 = branche2_2[urand(0, 2)];
					Spell_branche2_3 = branche2_3[urand(0, 2)];

					Random = urand(1, 3);
					if (Random == 1 && UpdateVictim()) { DoCastVictim(Spell_branche2_agro); }		// 1/3 Chance de lancer le sort d'agro
					break;

				}
			}
			void EnterEvadeMode(EvadeReason /*why*/) override
			{
				RetireBugDeCombat();
				me->AddUnitState(UNIT_STATE_EVADE);
				me->SetSpeedRate(MOVE_RUN, 1.5f);										// Vitesse de d�placement
				me->GetMotionMaster()->MoveTargetedHome();								// Retour home
			}
			void JustReachedHome() override
			{
				me->RemoveAura(Buf_branche1);
				me->RemoveAura(Buf_branche2);
				me->RemoveAura(Buf_all);

				me->SetReactState(REACT_AGGRESSIVE);
				me->SetSpeedRate(MOVE_RUN, 1.01f);									// Vitesse par defaut d�finit a 1.01f puisque le patch modification par type,famille test si 1.0f
				me->RemoveAura(Etreinte_Vampirique); 								// Retire Etreinte vampirique 15286
			}
			void UpdateAI(uint32 diff) override
			{
				// Emotes hors combat & mouvement -----------------------------------------------------------------------------------------------------------------
				if ((Cooldown_Npc_Emotes <= diff) && (!me->isMoving()) && (!me->IsInCombat()))
				{
					uint32 Npc_Play_Emotes = Npc_Emotes[urand(0, 21)];
					me->HandleEmoteCommand(Npc_Play_Emotes);
					Cooldown_Npc_Emotes = urand(8000, 15000);
				}
				else Cooldown_Npc_Emotes -= diff;

				// En Combat --------------------------------------------------------------------------------------------------------------------------------------
				if (!UpdateVictim() /*|| me->isPossessed() || me->IsCharmed() || me->HasAuraType(SPELL_AURA_MOD_FEAR)*/)
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				// Combat suivant la Sp�cialisation
				switch (BrancheSpe)
				{
				case 1: // Sp�cialisation Ombre ########################################################################################################################

					Mouvement_Caster(diff);
					Mouvement_All();
					Combat_Ombre(diff);
					break;

				case 2: // Sp�cialisation Discipline ##################################################################################################################

					Mouvement_Caster(diff);
					Mouvement_All();
					Heal_En_Combat_Heal(diff);
					Combat_Discipline(diff);
					break;

				}
			}

			void RetireBugDeCombat()
			{
				me->CombatStop(true);
				me->DeleteThreatList();
				me->LoadCreaturesAddon();
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);				// UNROOT
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);					// Retire flag "en combat"
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);				// Retire flag "non attaquable"
			}

			void Mouvement_All()
			{
				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());
				if ((Dist > DistanceDeCast) || (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 40))
				{
					RetireBugDeCombat();
					me->AddUnitState(UNIT_STATE_EVADE);
					EnterEvadeMode(EVADE_REASON_SEQUENCE_BREAK);						// Quite le combat si la cible > 30m (Caster & M�l�e) ou > 40m de home
				}
			}
			void Mouvement_Caster(uint32 diff)
			{
				if (!UpdateVictim() /*|| me->HasUnitState(UNIT_STATE_CASTING)*/)
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);
				if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT)) { me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT); }

				if (Cooldown_ResteADistance <= diff)
				{
					// Mouvement al�atoire si cible < 5m & Mana > 5% --------------------------------------------------------------------------------------------------

					if ((Dist <5) && (Mana > MaxMana / 20))
					{
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);						// UNROOT
						me->SetSpeedRate(MOVE_RUN, 1.1f);

						float x, y, z;
						x = (me->GetPositionX() + urand(0, ResteADistance * 2) - ResteADistance);
						y = (me->GetPositionY() + urand(0, ResteADistance * 2) - ResteADistance);
						z = me->GetPositionZ();
						me->GetMotionMaster()->MovePoint(0xFFFFFE, x, y, z);
						Cooldown_ResteADistance = urand(5000, 8000);
					}
				}
				else Cooldown_ResteADistance -= diff;

				// Speed normal si distance > 6m ------------------------------------------------------------------------------------------------------------------
				if (Dist> 6 && me->GetSpeedRate(MOVE_RUN) == 1.1f)
				{
					me->SetSpeedRate(MOVE_RUN, 1.01f);
				}

				// Mouvement OFF si Mana > 5% & distance >= 3m & <= 30m ---------------------------------------------------------------------------------------------
				if ((Mana > MaxMana / 20) && (Dist >= 3) && (Dist <= DistanceDeCast))
				{
					AttackStartCaster(victim, ResteADistance);											// Distance de combat
					void DoRangedAttackIfReady();														// Combat a distance
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);								// ROOT
				}

				// Mouvement ON si distance > 30m ------------------------------------------------------------------------------------------------------------------
				if (Dist > DistanceDeCast)
				{
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);							// UNROOT
				}

				// Mouvement ON si Mana < 5%  ----------------------------------------------------------------------------------------------------------------------
				if (Mana < MaxMana / 20)
				{
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);							// UNROOT
				}

			}

			void Combat_Ombre(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_MOVE) /*|| me->HasUnitState(UNIT_STATE_CASTING)*/)
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				if (Dist <= DistanceDeCast)
				{
					// Regen mana en combat ------------------------------------------------------------------------------------------------------------------------
					if (Cooldown_RegenMana <= diff)
					{
						me->SetPower(POWER_MANA, Mana + (MaxMana / 10));
						if (Mana > MaxMana) { me->SetPower(POWER_MANA, MaxMana); }
						Cooldown_RegenMana = urand(2500,3000);
					}
					else Cooldown_RegenMana -= diff;

					// Combat --------------------------------------------------------------------------------------------------------------------------------------
					// Spell3 sur la cible  (Sort secondaire tres lent , g�n�ralement utilis� comme Dot)
					if (Cooldown_Spell3 <= diff)
					{
						DoCastVictim(Spell_branche1_3);
						Cooldown_Spell3 = urand(10000, 12000);
					}
					else Cooldown_Spell3 -= diff;

					// Spell2 sur la cible chaque (Sort secondaire plus lent)
					if (Cooldown_Spell2 <= diff /*&& !me->HasUnitState(UNIT_STATE_CASTING)*/)
					{
						DoCastVictim(Spell_branche1_2);
						Cooldown_Spell2 = 5000;
					}
					else Cooldown_Spell2 -= diff;

					// Spell1 sur la cible chaque (Sort R�guli�)
					if (Cooldown_Spell1 <= diff)
					{
						DoCastVictim(Spell_branche1_1);
						Cooldown_Spell1 = 4000;
					}
					else Cooldown_Spell1 -= diff;

				}
			}
			void Combat_Discipline(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_MOVE) /*|| me->HasUnitState(UNIT_STATE_CASTING)*/)
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				if (Dist <= DistanceDeCast)
				{

				// Regen mana en combat ------------------------------------------------------------------------------------------------------------------------
				if (Cooldown_RegenMana <= diff)
				{
					me->SetPower(POWER_MANA, Mana + (MaxMana / 10));
					if (Mana > MaxMana) { me->SetPower(POWER_MANA, MaxMana); }
					Cooldown_RegenMana = urand(2000, 2500);
				}
				else Cooldown_RegenMana -= diff;

				// Combat --------------------------------------------------------------------------------------------------------------------------------------
					// Spell1 sur la cible chaque (Sort R�guli�)
				if (Cooldown_Spell1 <= diff)
				{
					DoCastVictim(Spell_branche2_1);
					Cooldown_Spell1 = 3500;
				}
				else Cooldown_Spell1 -= diff;

				// Spell2 sur la cible chaque (Sort secondaire plus lent)
				if (Cooldown_Spell2 <= diff)
				{
					DoCastVictim(Spell_branche2_2);
					Cooldown_Spell2 = 8000;
				}
				else Cooldown_Spell2 -= diff;

				// Spell3 sur la cible  (Sort secondaire tres lent , g�n�ralement utilis� comme Dot)
				if (Cooldown_Spell3 <= diff)
				{
					DoCastVictim(Spell_branche2_3);
					Cooldown_Spell3 = urand(8000, 10000);
				}
				else Cooldown_Spell3 -= diff;
				}
			}

			void Heal_En_Combat_Caster(uint32 diff)
			{
				if (Cooldown_Spell_Heal <= diff)
				{
					Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, DistanceDeCast, true);		// pour heal friend

					// heal sur lui meme-------------------------------------------------------------------------------------------------------------------------------
					if ((me->GetHealth() < (me->GetMaxHealth()*0.60)))								// Si PV < 60%
					{
						DoCast(me, Spell_Heal_Caster);
						Cooldown_Spell_Heal = 3500;
					}

					// heal sur Friend --------------------------------------------------------------------------------------------------------------------------------
					if (target = DoSelectLowestHpFriendly(DistanceDeCast))							// Distance de l'alli� = 30m
					{
						if (me->IsFriendlyTo(target) && (me != target))
						{
							if (target->GetHealth() < (target->GetMaxHealth()*0.40))				// Si PV < 40%
							{
								DoCast(target, Spell_Heal_Caster);
								Cooldown_Spell_Heal = 4000;
							}
						}
					}
				}
				else Cooldown_Spell_Heal -= diff;
			}
			void Heal_En_Combat_Heal(uint32 diff)
			{
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, DistanceDeCast, true);			// pour heal friend

				// mot de pouvoir:bouclier sur lui meme ----------------------------------------------------------------------------------------------------------------
				if (Cooldown_Spell_Bouclier <= diff)
				{
					if (me->GetHealth() < (me->GetMaxHealth()*0.40))								// Si PV < 40%
					{
						me->CastSpell(me, Mot_de_pouvoir_Bouclier, true);							// Utilise Mot de pouvoir : Bouclier chaque 30s 
						Cooldown_Spell_Bouclier = 30000;
					}
				} else Cooldown_Spell_Bouclier -= diff;

				// Heal sur lui & Friend
				if (Cooldown_Spell_Heal <= diff)
				{
					// heal sur lui meme ------------------------------------------------------------------------------------------------------------------------------
					if (me->GetHealth() < (me->GetMaxHealth()*0.60) )								// Si PV < 60%
					{
						me->CastSpell(me, Spell_Heal_Heal,true);

						if ( me->GetHealth() < (me->GetMaxHealth()*0.40) )							// Si PV < 40%
						{
							DoCast(me, Soins_Rapides);												// Soins rapides 2061 Soins_Rapides
						}
						Cooldown_Spell_Heal = 3000;
					}

					// heal sur Friend ---------------------------------------------------------------------------------------------------------------------------------
					else if (target = DoSelectLowestHpFriendly(DistanceDeCast))						// Distance de l'alli� = 30m
					{
						if (me->IsFriendlyTo(target) && (me != target))
						{
							if (target->GetHealth() < (target->GetMaxHealth()*0.60))				// Si PV < 60%
							{
								me->CastSpell(target, Spell_Heal_Heal, true);;
							}
							if (target->GetHealth() < (target->GetMaxHealth()*0.40) )				// Si PV < 40%
							{
									DoCast(target, Soins_Rapides);									// 1 chance sur 2 : Soins rapides 2061 Soins_Rapides
							}
							Cooldown_Spell_Heal = 4000;
						}
					}
				}
				else Cooldown_Spell_Heal -= diff;
			}

		};
		
		CreatureAI* GetAI(Creature* creature) const override
		{
			return new Stitch_npc_ai_pretreAI(creature);
		}
};

//##################################################################################################################################################################
void AddSC_npc_ai_pretre()
{
	new Stitch_npc_ai_pretre;
}















