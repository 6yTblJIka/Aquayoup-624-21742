////#########################################################################################################################################################################################################################################
// Copyright (C) Juin 2020 Stitch pour Aquayoup
// AI generique npc par classe : MAGE V1.0
// Il est possible d'influencer le temp entre 2 cast avec `BaseAttackTime` & `RangeAttackTime` 
// Necessite dans Creature_Template :
// Minimun  : UPDATE `creature_template` SET `ScriptName` = 'Stitch_npc_ai_mage',`AIName` = '' WHERE (entry = 15100005);
// Optionel : UPDATE `creature_template` SET `HealthModifier` = 2, `ManaModifier` = 3, `ArmorModifier` = 1, `DamageModifier` = 2,`BaseAttackTime` = 2000, `RangeAttackTime` = 2000 WHERE(entry = 15100005);
// Optionel : Utilisez pickpocketloot de creature_template pour passer certains parametres (Solution choisit afin de rester compatible avec tout les cores). Si pickpocketloot = 1 (branche1 forc�), pickpocketloot = 2 (branche2 forc�), etc
//###########################################################################################################################################################################################################################################
// # npc de Test Stitch_npc_ai_Mage  .npc 15100005
// REPLACE INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `femaleName`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_unk`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `HealthModifier`, `HealthModifierExtra`, `ManaModifier`, `ManaModifierExtra`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
// (15100005, 0, 0, 0, 0, 0, 28759, 0, 0, 0, 'npc_ai_Mage', '', '', '', 0, 90, 90, 0, 0, 2102, 0, 1.01, 1.01, 1, 0, 0, 2000, 2000, 1, 1, 2, 0, 2048, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 2, 1, 3, 1, 2, 2, 1, 0, 144, 1, 0, 0, 'Stitch_npc_ai_mage', 20173);
// REPLACE INTO `creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES
// (15100005, 1, 854, 0, 0, 0),
// (15100005, 2, 854, 0, 0, 0),
// (15100005, 3, 854, 0, 0, 0);
//###########################################################################################################################################################################################################################################



//################################################################################################
//StitchAI AI Mage
//################################################################################################

class Stitch_npc_ai_mage : public CreatureScript
{
public: Stitch_npc_ai_mage() : CreatureScript("Stitch_npc_ai_mage") { }

		struct Stitch_npc_ai_mageAI : public ScriptedAI
		{
			Stitch_npc_ai_mageAI(Creature* creature) : ScriptedAI(creature) { }

			uint32 BrancheSpe = 1;													// Choix de la Sp�cialisation : Arcane = 1, Feu = 2, Givre = 3
			uint32 NbrDeSpe = 3;													// Nombre de Sp�cialisations 
			uint32 ForceBranche;
			uint32 Random;
			uint32 DistanceDeCast = 30;												// Distance max a laquelle un npc attaquera , au dela il quite le combat
			uint32 ResteADistance = 10;												// Distance max a laquelle un npc s'approchera
			uint32 Dist;															// Distance entre le npc et sa cible
			uint32 Mana;
			uint32 MaxMana = me->GetMaxPower(POWER_MANA);

			Unit* victim = me->GetVictim();

			// Definitions des variables Cooldown et le 1er lancement
			uint32 Cooldown_Spell1 = 500;
			uint32 Cooldown_Spell2 = 2000;
			uint32 Cooldown_Spell3 = 3500;
			uint32 Cooldown_Spell4 = 5500;
			uint32 Cooldown_Spell_Heal = 5000;											// Cooldown pour la fr�quence du heal
			uint32 Cooldown_RegenMana = 3000;											// Cooldown pour le regen du mana
			uint32 Cooldown_ResteADistance = 4000;										// Test si en contact pour s'eloigner
			uint32 Cooldown_Npc_Emotes = urand(5000, 8000);

			// Spells Divers
			uint32 Buf_all = 165743;													// Armure de givre 165743, Armure du mage 6118
			uint32 Buf_branche1;	
			uint32 Buf_branche1_liste[2] = { 1459, 36749 };								// Illumination des Arcanes 1459, Puissance arcanique 36749 (Puissance des sorts +25/1h)
			uint32 Buf_branche2 ;
			uint32 Buf_branche2_liste[2] = { 30482, 140468 };							// Armure de la fournaise 30482 (physique -10%, Lueur de la flamme 140468 (dmg -20%)
			uint32 Buf_branche3 ;
			uint32 Buf_branche3_liste[2] = { 111264, 1459 };							// Garde glaciale 111264 (3 m�l�es 30s), Illumination des Arcanes 1459
			uint32 Spell_Heal_Caster = 12051;											// Evocation 12051

			// Spells Arcane
			uint32 Spell_branche1_agro;
			uint32 Spell_branche1_1;
			uint32 Spell_branche1_2;
			uint32 Spell_branche1_3;
			uint32 Spell_branche1_4;
			uint32 branche1_agro[3] = { 30449, 102051, 31589 };							// Vol de sort 30449, Givregueule 102051 (silence 8s), Lenteur 31589
			uint32 branche1_1[2] = { 44425, 44425 };									// Barrage des Arcanes 44425
			uint32 branche1_2[2] = { 30451, 30451 };									// D�flagration des Arcanes 30451
			uint32 branche1_3[2] = { 5143, 5143 };										// Projectiles des Arcanes 5143 ,�clair des Arcanes (rose) 38204

			// Spells Feu
			uint32 Spell_branche2_agro;    
			uint32 Spell_branche2_1;
			uint32 Spell_branche2_2;
			uint32 Spell_branche2_3;
			uint32 Spell_branche2_4 = 31661;											// Souffle du dragon 31661 (Recharge 20s,12m)
			uint32 branche2_agro[4] = { 2120, 30449, 102051 , 2139 };					// Choc de flammes 2120, Vol de sort 30449, Givregueule 102051 (silence 8s), Contresort 2139
			uint32 branche2_1[2] = { 133, 133 };										// Boule de feu 133 
			uint32 branche2_2[2] = { 2136, 2948 };										// Trait de feu 2136 , Br�lure 2948 
			uint32 branche2_3[2] = { 108853, 108853 };									// D�flagration infernale 108853

			// Spells Givre
			uint32 Spell_branche3_agro;  
			uint32 Spell_branche3_1;
			uint32 Spell_branche3_2;
			uint32 Spell_branche3_3;
			uint32 branche3_agro[3] = { 140485, 84714, 2139 };							// Pi�ge � runes gel� 140485 (gele 20s,20m), Sol gel� 84714, Contresort 2139, Invocation d'un �l�mentaire d'eau 31687
			uint32 branche3_1[2] = { 116, 116 };										// Eclair de givre 116
			uint32 branche3_2[2] = { 30455, 30455 };									// Javelot de glace 30455
			uint32 branche3_3[2] = { 120, 122 };										// C�ne de froid 120 (Recharge 12s,12m) , Nova de givre 122
			
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
				ForceBranche = me->GetCreatureTemplate()->pickpocketLootId;											// creature_template->pickpocketloot
				if (ForceBranche == 1) { BrancheSpe = 1; }															// branche1 forc�
				else if (ForceBranche == 2) { BrancheSpe = 2; }														// branche2 forc� 
				else if (ForceBranche == 3) { BrancheSpe = 3; }														// branche2 forc�
				else
				{
					// Sinon Choix de la Sp�cialisation Al�atoire
					BrancheSpe = urand(1, NbrDeSpe);
				}

				if ((BrancheSpe > NbrDeSpe) || (BrancheSpe == 0)) { BrancheSpe = 2; }

				// Spell a lancer a l'agro ------------------------------------------------------------------------------------------------------------------------
				me->CastSpell(me, Buf_all, true);																	// Buf_all sur lui meme pour toutes les Sp�cialit�es

				switch (BrancheSpe)
				{
				case 1: // Si Arcane --------------------------------------------------------------------------------------------------------------------------
					Buf_branche1 = Buf_branche1_liste[urand(0, 1)];
					me->CastSpell(me, Buf_branche1, true);															// Buf1 sur lui meme

					me->LoadEquipment(1, true);																		// creature_equip_template 1

					// Tirages al�atoires des spells Arcane 
					Spell_branche1_agro = branche1_agro[urand(0, 2)];
					Spell_branche1_1 = branche1_1[urand(0, 1)];
					Spell_branche1_2 = branche1_2[urand(0, 1)];
					Spell_branche1_3 = branche1_3[urand(0, 1)];

					Random = urand(1, 2);
					if (Random == 1 && UpdateVictim()) { DoCastVictim(Spell_branche1_agro); }						// 1/2 Chance de lancer le sort d'agro
					break;

				case 2: // Si Feu -------------------------------------------------------------------------------------------------------------------------
					Buf_branche2 = Buf_branche2_liste[urand(0, 1)];
					me->CastSpell(me, Buf_branche2, true);															// Buf2 sur lui meme

					me->LoadEquipment(2, true);																		// creature_equip_template 2

					// Tirages al�atoires des spells Feu 
					Spell_branche2_agro = branche2_agro[urand(0, 3)];
					Spell_branche2_1 = branche2_1[urand(0, 1)];
					Spell_branche2_2 = branche2_2[urand(0, 1)];
					Spell_branche2_3 = branche2_3[urand(0, 1)];

					Random = urand(1, 2);
					if (Random == 1 && UpdateVictim()) { DoCastVictim(Spell_branche2_agro); }						// 1/2 Chance de lancer le sort d'agro
					break;

				case 3: // Si Givre -------------------------------------------------------------------------------------------------------------------------
					Buf_branche3 = Buf_branche3_liste[urand(0, 1)];
					me->CastSpell(me, Buf_branche3, true);															// Buf2 sur lui meme

					me->LoadEquipment(3, true);																		// creature_equip_template 3

					// Tirages al�atoires des spells Givre 
					Spell_branche3_agro = branche3_agro[urand(0, 3)];
					Spell_branche3_1 = branche3_1[urand(0, 1)];
					Spell_branche3_2 = branche3_2[urand(0, 1)];
					Spell_branche3_3 = branche3_3[urand(0, 1)];

					Random = urand(1, 2);
					if (Random == 1 && UpdateVictim()) { me->CastSpell(victim, Spell_branche3_agro, true); }		// 1/2 Chance de lancer le sort d'agro

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
				me->RemoveAura(Buf_branche3);
				me->RemoveAura(Buf_all);

				me->SetReactState(REACT_AGGRESSIVE);
				me->SetSpeedRate(MOVE_RUN, 1.01f);										// Vitesse par defaut d�finit a 1.01f puisque le patch modification par type,famille test si 1.0f
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
				case 1: // Sp�cialisation Arcane ##################################################################################################################

					Mouvement_All();
					Mouvement_Caster(diff);
					Heal_En_Combat_Caster(diff);
					Combat_Arcane(diff);
					break;
					
				case 2: // Sp�cialisation Feu #####################################################################################################################
					
					Mouvement_All();
					Mouvement_Caster(diff);
					Heal_En_Combat_Caster(diff);
					Combat_Feu(diff);
					break;

				case 3: // Sp�cialisation Givre ###################################################################################################################
					
					Mouvement_All();
					Mouvement_Caster(diff);
					Heal_En_Combat_Caster(diff); 
					Combat_Givre(diff);
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
					AttackStart(victim); 
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

			void Combat_Arcane(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_MOVE) /*|| me->HasUnitState(UNIT_STATE_CASTING)*/)
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

				if (Dist <= DistanceDeCast)
				{
					// Regen mana en combat ------------------------------------------------------------------------------------------------------------------------
					if (Cooldown_RegenMana <= diff)
					{
						me->SetPower(POWER_MANA, Mana + (MaxMana / 12));
						if (Mana > MaxMana) { me->SetPower(POWER_MANA, MaxMana); }
						Cooldown_RegenMana = urand(2500,3000);
					}
					else Cooldown_RegenMana -= diff;

					// Combat --------------------------------------------------------------------------------------------------------------------------------------

					// Spell3 sur la cible 
					if (Cooldown_Spell3 <= diff)
					{
						me->CastSpell(victim, Spell_branche1_3, true);
						Cooldown_Spell3 = 10000;
					} else Cooldown_Spell3 -= diff;

					// Spell2 sur la cible 
					if (Cooldown_Spell2 <= diff)
					{
						DoCastVictim(Spell_branche1_2);
						Cooldown_Spell2 = 4000;
					}
					else Cooldown_Spell2 -= diff;

					// Spell1 sur la cible 
					if (Cooldown_Spell1 <= diff)
					{
						DoCastVictim(Spell_branche1_1);
						Cooldown_Spell1 = 4000;
					}
					else Cooldown_Spell1 -= diff;

				}
			}
			void Combat_Feu(uint32 diff)
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
						me->SetPower(POWER_MANA, Mana + (MaxMana / 12));
						if (Mana > MaxMana) { me->SetPower(POWER_MANA, MaxMana); }
						Cooldown_RegenMana = urand(2000, 2500);
					}
					else Cooldown_RegenMana -= diff;

					// Combat --------------------------------------------------------------------------------------------------------------------------------------
						// Spell1 sur la cible chaque (Sort R�guli�)
					if (Cooldown_Spell1 <= diff && !me->HasUnitState(UNIT_STATE_MOVE)  )
					{
						DoCastVictim(Spell_branche2_1);
						Cooldown_Spell1 = 3000;
					}
					else Cooldown_Spell1 -= diff;

					// Spell2 sur la cible 
					if (Cooldown_Spell2 <= diff )
					{
						DoCastVictim(Spell_branche2_2);
						Cooldown_Spell2 = urand(2500,3000);
					}
					else Cooldown_Spell2 -= diff;

					// Spell3 sur la cible 
					if (Cooldown_Spell3 <= diff)
					{
						DoCastVictim(Spell_branche2_3);
						Cooldown_Spell3 = urand(6000, 8000);
					}
					else Cooldown_Spell3 -= diff;

					// Spell4 sur la cible 
					if (Cooldown_Spell4 <= diff && Dist <= 10)
					{
						DoCastVictim(Spell_branche2_4);
						Cooldown_Spell4 = urand(12000, 14000);
					}
					else Cooldown_Spell4 -= diff;
				}
			}
			void Combat_Givre(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_MOVE) /*|| me->HasUnitState(UNIT_STATE_CASTING)*/)
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				// Regen mana en combat ------------------------------------------------------------------------------------------------------------------------
				if (Cooldown_RegenMana <= diff)
				{
					me->SetPower(POWER_MANA, Mana + (MaxMana / 15));
					if (Mana > MaxMana) { me->SetPower(POWER_MANA, MaxMana); }
					Cooldown_RegenMana = urand(2000, 2500);
				}
				else Cooldown_RegenMana -= diff;

				// Combat --------------------------------------------------------------------------------------------------------------------------------------
				// Spell1 sur la cible chaque (Sort R�guli�)
				if (Cooldown_Spell1 <= diff && !me->HasUnitState(UNIT_STATE_MOVE))
				{
					DoCastVictim(Spell_branche3_1, true);
					Cooldown_Spell1 = 3000;
				}
				else Cooldown_Spell1 -= diff;

				// Spell2 sur la cible chaque (Sort secondaire plus lent)
				if (Cooldown_Spell2 <= diff)
				{
					DoCastVictim(Spell_branche3_2, true);
					Cooldown_Spell2 = 2500;
				}
				else Cooldown_Spell2 -= diff;

				// Spell3 sur la cible  (Sort secondaire tres lent , g�n�ralement utilis� comme Dot)
				if (Cooldown_Spell3 <= diff && Dist <= 10)
				{
					DoCastVictim(Spell_branche3_3, true);
					Cooldown_Spell3 = urand(10000, 12000);
				}
				else Cooldown_Spell3 -= diff;








			}

			void Heal_En_Combat_Caster(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_MOVE))
					return;

				if (Cooldown_Spell_Heal <= diff)
				{
					// heal sur lui meme-------------------------------------------------------------------------------------------------------------------------------
					if ((me->GetHealth() < (me->GetMaxHealth()*0.50)))								// Si PV < 50%
					{
						me->InterruptNonMeleeSpells(true);
						DoCast(me, Spell_Heal_Caster);
						Cooldown_Spell_Heal = 60000;
					}
				}
				else Cooldown_Spell_Heal -= diff;
			}



		};
		
		CreatureAI* GetAI(Creature* creature) const override
		{
			return new Stitch_npc_ai_mageAI(creature);
		}
};

//##################################################################################################################################################################
void AddSC_npc_ai_mage()
{
	new Stitch_npc_ai_mage;
}














