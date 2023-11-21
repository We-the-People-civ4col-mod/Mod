## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil

DebugLogging = False

gc = CyGlobalContext()

class CvDiplomacy:
	"Code used by Civ Diplomacy interface"

	def __init__(self):
		"constructor - set up class vars, AI and User strings"
		if DebugLogging:
			print "Launching Diplomacy"

		self.iLastResponseID = -1

		self.diploScreen = CyDiplomacy()

	def setDebugLogging(self, bDebugLogging):
		global DebugLogging
		DebugLogging = bDebugLogging

	def determineResponses (self, eComment, *args):
		"Will determine the user responses given an AI comment"
		if DebugLogging:
			print "CvDiplomacy.determineResponses: %s" %(eComment,)

		# set up iData1+2 from args if enough arguments are given
		# note: more arguments can be given, but then args[0][x] will be needed as they aren't named here
		iData1 = -1
		iData2 = -1
		if (len(args) >= 1):
			num_args = len(args[0])
			if (num_args >= 1):
				iData1 = args[0][0]
				if (num_args >= 2):
					iData2 = args[0][1]

		# Eliminate previous comments
		self.diploScreen.clearUserComments()

		# If the AI is declaring war
		if (self.isComment(eComment, "AI_DIPLOCOMMENT_DECLARE_WAR") ):

			# We respond to their declaration
			self.addUserComment("USER_DIPLOCOMMENT_WAR_RESPONSE", -1, -1)
			self.diploScreen.endTrade()

		# If this is the first time we are being contacted by the AI
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_FIRST_CONTACT")):

			# We say hi and begin our peace
			self.addUserComment("USER_DIPLOCOMMENT_PEACE", -1, -1)

			# if you are on different teams and NOT at war, give the user the option to declare war
			#if (gc.getTeam(gc.getGame().getActiveTeam()).canDeclareWar(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam())):
			#	self.addUserComment("USER_DIPLOCOMMENT_WAR", -1, -1)

			self.diploScreen.endTrade()

		# The AI refuses to talk
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_REFUSE_TO_TALK") ):

			# Give the option to exit
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
			self.diploScreen.endTrade();

		# If the AI is offering a city oo
		# If the AI is giving help
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_OFFER_CITY") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_GIVE_HELP")):

			# We can accept their offer
			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_OFFER", -1, -1)
			# Or reject it...
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_OFFER", -1, -1)

		# If the AI is offering a deal
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_OFFER_PEACE") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_OFFER_DEAL")):

			# We can accept their offer
			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_OFFER", -1, -1)
			# Or we can try to negotiate
			self.addUserComment("USER_DIPLOCOMMENT_RENEGOTIATE", -1, -1)
			# Or reject it...
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_OFFER", -1, -1)

		# If the AI is cancelling a deal
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CANCEL_DEAL")):

			# We can try to renegotiate
			self.addUserComment("USER_DIPLOCOMMENT_RENEGOTIATE", -1, -1)
			# Or just exit...
			self.addUserComment("USER_DIPLOCOMMENT_NO_RENEGOTIATE", -1, -1)

		# If the AI is demanding tribute
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ASK_FOR_HELP")):

			# We can give them help
			self.addUserComment("USER_DIPLOCOMMENT_GIVE_HELP", -1, -1)
			# Or refuse...
			self.addUserComment("USER_DIPLOCOMMENT_REFUSE_HELP", -1, -1)

		# If the AI is demanding tribute
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_DEMAND_TRIBUTE")):

			# We can accept their demands
			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_DEMAND", -1, -1)
			# Or reject them...
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_DEMAND", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_BUY_UNITS")):

			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_BUY_UNITS", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_OFFER", -1, -1)

		# WTP, ray Kings Used Ship - START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_KING_OFFERS_USED_SHIP")):

			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_BUY_USED_SHIP", iData1, iData2)
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_OFFER", -1, -1)
		# WTP, ray Kings Used Ship - END
		
		# WTP, ray, Foreign Kings, buy Immigrants - START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_FOREIGN_KING_OFFERS_IMMIGRANTS")):

			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_FOREIGN_IMMIGRANTS", iData1, iData2)
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_OFFER", -1, -1)
		# WTP, ray, Foreign Kings, buy Immigrants - START

		# If the AI is raising taxes
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_KISS_PINKY")):
		
			# We can accept their demands
			self.addUserComment("USER_DIPLOCOMMENT_KISS_PINKY", -1, -1)

			# Or reject them...
			player = gc.getPlayer(gc.getGame().getActivePlayer())
			eYield = player.getHighestTradedYield()
			iCityId = player.getHighestStoredYieldCityId(eYield)
			city = player.getCity(iCityId)
			if city.isNone():
				szCityName = u""
			else:
				szCityName = city.getNameKey()

			self.addUserComment("USER_DIPLOCOMMENT_TAX_PARTY", eYield, -1, szCityName, gc.getYieldInfo(eYield).getTextKey())

		# accept pinky ring
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACCEPT_PINKY")):

			# We can accept their demands
			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_PINKY", -1, -1)

		# reject pinky ring
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_REJECT_PINKY")):

			player = gc.getPlayer(gc.getGame().getActivePlayer())
			eYield = player.getHighestTradedYield()
			iCityId = player.getHighestStoredYieldCityId(eYield)
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_PINKY", eYield, iCityId)

		# If the king is asking for gold
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_KING_ASK_FOR_GOLD")):

			# We can accept their demands
			self.addUserComment("USER_DIPLOCOMMENT_KING_GIVE_GOLD", -1, -1)
			# Or reject them...
			self.addUserComment("USER_DIPLOCOMMENT_KING_REFUSE_GOLD", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_KING_ACCEPT_GOLD")):

			# We can accept their demands
			self.addUserComment("USER_DIPLOCOMMENT_KING_ACCEPTED_GOLD", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_KING_REFUSE_GOLD")):

			self.addUserComment("USER_DIPLOCOMMENT_KING_REFUSED_GOLD", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_KING_REVIVE")):

			self.addUserComment("USER_DIPLOCOMMENT_KING_REVIVE", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_KING_GIFT_SHIP")):

			self.addUserComment("USER_DIPLOCOMMENT_KING_GIFT_SHIP", -1, -1)
			
		# If the AI is giving goodies to our scout
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CHIEF_GOODY")):

			# accept
			self.addUserComment("USER_DIPLOCOMMENT_CHIEF_GOODY", -1, -1)

		# If the AI is teaching our unit
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_LIVE_AMONG_NATIVES")):

			# accept
			self.addUserComment("USER_DIPLOCOMMENT_LIVE_AMONG_NATIVES_ACCEPT", -1, -1)

			# reject
			self.addUserComment("USER_DIPLOCOMMENT_LIVE_AMONG_NATIVES_REJECT", -1, -1)

		# If the AI is offering to transport treasure
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_TREASURE_TRANSPORT")):

			# accept
			self.addUserComment("USER_DIPLOCOMMENT_TREASURE_TRANSPORT_ACCEPT", -1, -1)

			# reject
			self.addUserComment("USER_DIPLOCOMMENT_TREASURE_TRANSPORT_REJECT", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_FOUND_CITY_NO_FOOD")):

			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_NO_FOOD_YES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_NO_FOOD_NO", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_FOUND_CITY_INLAND")):

			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_INLAND_YES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_INLAND_NO", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_FOUND_FIRST_CITY")):

			self.addUserComment("USER_DIPLOCOMMENT_FOUND_FIRST_CITY", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_FOUND_CITY")):

			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_BUY", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_TAKE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_CANCEL", -1, -1)
			

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_FOUND_CITY_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_TAKE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_FOUND_CITY_CANCEL", -1, -1)
			
		# ray18
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_CITY")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_CITY_BUY", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_CITY_TAKE", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_CITY_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_CITY_TAKE", -1, -1)
		# Ende ray18
		
		#TAC Native Mercs
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_MERC")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_MERC", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_MERC_CHEAP", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_MERC_CANT_AFFORD", -1, -1)
		
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_MERC_ONLY_CHEAP")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_MERC_CHEAP", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_MERC_CANT_AFFORD", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_MERC_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_MERC_CANT_AFFORD", -1, -1)
		# TAC Native Mercs
		
		# R&R, ray, Native Slave, START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE_CANT_AFFORD", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE_CANT_AFFORD", -1, -1)
		#  R&R, ray, Native Slave, END
		
		# R&R, ray, Native Slave, START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES_CANT_AFFORD", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES_CANT_AFFORD", -1, -1)
		#  R&R, ray, Native Slave, END
		
		# RaR, ray Prisons Crowded
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_PRISONS_CROWDED")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_PRISONS", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_PRISONS_CANT_AFFORD", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_PRISONS_CROWDED_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_PRISONS_CANT_AFFORD", -1, -1)
		# RaR, ray Prisons Crowded
		
		# RaR, ray Revolutionary Noble
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_REVOLUTIONARY_NOBLE")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_NOBLE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_NOBLE_CANT_AFFORD", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_REVOLUTIONARY_NOBLE_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_NOBLE_CANT_AFFORD", -1, -1)
		# RaR, ray Revolutionary Noble
		
		# RaR, ray Bishop
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_BISHOP")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_BISHOP", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_BISHOP_CANT_AFFORD", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_BISHOP_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_BISHOP_CANT_AFFORD", -1, -1)
		# RaR, ray Bishop
		
		# RaR, ray Church
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CHURCH_DEMAND")):

			self.addUserComment("USER_DIPLOCOMMENT_PAY_CHURCH_DEMAND", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_REFUSE_CHURCH_DEMAND", -1, -1)
		# RaR, ray Church
		
		# RaR, ray Church Favours
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CHURCH_FAVOUR")):
			iPrice = gc.getPlayer(gc.getGame().getActivePlayer()).getChurchFavourPrice()
			if (gc.getPlayer(gc.getGame().getActivePlayer()).getGold() >= iPrice):
				self.addUserComment("USER_DIPLOCOMMENT_CHURCH_FAVOUR_KING", -1, -1)
				for i in range(gc.getMAX_CIV_PLAYERS()):
					if (gc.getPlayer(i).isAlive() and not gc.getGame().isBarbarianPlayer(i) and not gc.getPlayer(i).isNative() and not gc.getPlayer(i).isEurope() and (i != gc.getGame().getActivePlayer()) and (i != self.diploScreen.getWhoTradingWith())):
						if (gc.getTeam(gc.getGame().getActiveTeam()).isHasMet(gc.getPlayer(i).getTeam()) and gc.getTeam(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()).isHasMet(gc.getPlayer(i).getTeam())):
							self.addUserComment("USER_DIPLOCOMMENT_CHURCH_FAVOUR_EUROPEAN", i, -1, gc.getPlayer(i).getNameKey())
			#self.addUserComment("USER_DIPLOCOMMENT_CHURCH_FAVOUR_FOUNDING_FATHERS", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
		# RaR, ray Church Favours
		
		# RaR, ray, Smuggling
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_SMUGGLERS")):

			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_SMUGGLERS", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_SMUGGLERS", -1, -1)
		# RaR, ray, Smuggling
		
		# RaR, ray, Pirates
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_PIRATES")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_PIRATES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_BRIBE_PIRATES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_PIRATES", -1, -1)
		# RaR, ray, Pirates
		
		# RaR, ray, Continental Guard
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_DONATE_CONTINENTAL_GUARD")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_CONTINENTAL_GUARD", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_CONTINENTAL_GUARD", -1, -1)
		# RaR, ray, Continental Guard
		
		# RaR, ray, Mortar
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_DONATE_MORTAR")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_MORTAR", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_MORTAR", -1, -1)
		# RaR, ray, Mortar
		
		# RaR, ray, Rangers
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_RANGERS")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_RANGERS", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_RANGERS", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_RANGERS_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_RANGERS", -1, -1)
		# RaR, ray, Rangers
		
		# RaR, ray, Conquistadors
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CONQUISTADORS")):

			self.addUserComment("USER_DIPLOCOMMENT_ACQUIRE_CONQUISTADORS", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_CONQUISTADORS", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CONQUISTADORS_CANT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_NOT_ACQUIRE_CONQUISTADORS", -1, -1)
		# RaR, ray, Conquistadors
		
		# TAC Revolution Support
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_GET_REV_SUPPORT")):

			self.addUserComment("USER_DIPLOCOMMENT_REV_NEED_LAND", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_REV_NEED_SEA", -1, -1)
		# TAC Revolution Support
		
		# TAC European Wars
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_EUROPE_WAR")):

			self.addUserComment("USER_DIPLOCOMMENT_EUROPE_WAR_YES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EUROPE_WAR_YES_BUT", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EUROPE_WAR_NO", -1, -1)
		# TAC European Wars
		
		# RaR, ray Stealing Immigrants
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_STEALING_IMMIGRANT")):

			self.addUserComment("USER_DIPLOCOMMENT_STEALING_IMMIGRANT_YES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_STEALING_IMMIGRANT_NO", -1, -1)
			
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_STEALING_IMMIGRANT_CANT_AFFORD")):
		
			self.addUserComment("USER_DIPLOCOMMENT_STEALING_IMMIGRANT_NO", -1, -1)
		# RaR, ray Stealing Immigrants
		
		# RWL European Peace
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_EUROPE_PEACE")):

			self.addUserComment("USER_DIPLOCOMMENT_EUROPE_PEACE_YES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EUROPE_PEACE_NO", -1, -1)
		# RWL European Peace
		
		# WTP, ray, Royal Intervention, START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ROYAL_INTERVENTION")):

			self.addUserComment("USER_DIPLOCOMMENT_ROYAL_INTERVENTION_YES_GOLD", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ROYAL_INTERVENTION_YES_TAX_INCREASE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_ROYAL_INTERVENTION_NO", -1, -1)
		# WTP, ray, Royal Intervention, END
		
		# WTP, ray, Privateers DLL Diplo Event - START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_PRIVATEERS_ACCUSSATION")):

			self.addUserComment("USER_DIPLOCOMMENT_PRIVATEERS_ADMIT", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_PRIVATEERS_DENY", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_PRIVATEERS_WAR", -1, -1)
		# WTP, ray, Privateers DLL Diplo Event - END
		
		# RaR, ray Church Wars
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CHURCH_WAR")):

			self.addUserComment("USER_DIPLOCOMMENT_CHURCH_WAR_YES", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_CHURCH_WAR_YES_BUT", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_CHURCH_WAR_NO", -1, -1)
		# RaR, ray Church Wars

		# WTP, ray, Colonial Intervention In Native War - START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_COLONIAL_INTERVENTION_NATIVE_WAR")):

			self.addUserComment("USER_DIPLOCOMMENT_COLONIAL_INTERVENTION_NATIVE_WAR_END", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_COLONIAL_INTERVENTION_NATIVE_WAR_CONTINUE", -1, -1)
		# WTP, ray, Colonial Intervention In Native War - END

		# WTP, ray, Big Colonies and Native Allies War - START
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_COLONIES_AND_NATIVE_ALLIES_WAR")):

			self.addUserComment("USER_DIPLOCOMMENT_COLONIES_AND_NATIVE_ALLIES_WAR_REFUSE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_COLONIES_AND_NATIVE_ALLIES_WAR_ACCEPT", -1, -1)
		# WTP, ray, Big Colonies and Native Allies War - END

		# If the AI is gifting some goods (yields)
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_NATIVES_YIELD_GIFT")):

			# accept
			self.addUserComment("USER_DIPLOCOMMENT_NATIVES_YIELD_GIFT_ACCEPT", -1, -1)
			
		# R&R, ray, Natives Trading
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_NATIVE_TRADE")):

			self.addUserComment("USER_DIPLOCOMMENT_NATIVES_TRADE_ACCEPT", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_NATIVES_TRADE_DECLINE", -1, -1)
		
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_NATIVE_TRADE_CANNOT_AFFORD")):

			self.addUserComment("USER_DIPLOCOMMENT_NATIVES_TRADE_DECLINE", -1, -1)
		# R&R, ray, Natives Trading

		# If the AI is pressuring us to join their war
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_JOIN_WAR")):

			# We can accept their demands
			self.addUserComment("USER_DIPLOCOMMENT_JOIN_WAR", -1, -1)
			# Or reject them...
			self.addUserComment("USER_DIPLOCOMMENT_NO_JOIN_WAR", -1, -1)

		# If the AI is pressuring us to stop trading with their enemy
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_STOP_TRADING")):

			# We can accept their demands
			self.addUserComment("USER_DIPLOCOMMENT_STOP_TRADING", -1, -1)
			# Or reject them...
			self.addUserComment("USER_DIPLOCOMMENT_NO_STOP_TRADING", -1, -1)

		# If we are viewing our current deals or
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_CURRENT_DEALS")):

			# Exit option
			self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
			self.diploScreen.startTrade( eComment, True)

		#RWL Bargaining Start 
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_BARGAIN_BUY_SUCCESS")):
		
			# Erik: This prevents the exploit where the player adds
			# additional items to the table after the AI has accepted the bargain
			if (self.diploScreen.isAIOffer()):
				self.addUserComment("USER_DIPLOCOMMENT_BARGAIN_BUY", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_BARGAIN_ACCEPT", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
			else:
				self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
									
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_BARGAIN_SELL_SUCCESS")):

			if (self.diploScreen.isAIOffer()):
				self.addUserComment("USER_DIPLOCOMMENT_BARGAIN_SELL", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_BARGAIN_ACCEPT", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
			else:
				self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
				self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
				
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_BARGAIN_FAIL")):
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
		#RWL Bargaining End
			
		# If we are trading or
		# If we are trying another proposal or
		# If they reject our offer or
		# If they reject our demand
		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_TRADING") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_REJECT") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_SORRY") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_TRY_THIS_DEAL") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_TRY_THIS_DEAL_BUY") or #RWL Bargaining Changed 
					self.isComment(eComment, "AI_DIPLOCOMMENT_TRY_THIS_DEAL_SELL") or #RWL Bargaining Changed 
					self.isComment(eComment, "AI_DIPLOCOMMENT_NO_DEAL") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_REJECT_ASK") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_REJECT_DEMAND")):

			# If no one is currently offering anything
			if (self.diploScreen.ourOfferEmpty() == 1 and self.diploScreen.theirOfferEmpty() == 1):

				# If we are at war, allow us to suggest a peace treaty
				if (self.diploScreen.atWar()):
					self.addUserComment("USER_DIPLOCOMMENT_PROPOSE", -1, -1)
					self.addUserComment("USER_DIPLOCOMMENT_OFFER_PEACE", -1, -1)

			# If one of us has something on the table
			if (self.diploScreen.ourOfferEmpty() == 0 or self.diploScreen.theirOfferEmpty() == 0):

				# If the offer is from the AI
				if (self.diploScreen.isAIOffer()):

					# We can accept or reject the offer
					self.addUserComment("USER_DIPLOCOMMENT_ACCEPT", -1, -1)
					self.addUserComment("USER_DIPLOCOMMENT_REJECT", -1, -1)
					
					#RWL Bargaining Start 
					if (self.isComment(eComment, "AI_DIPLOCOMMENT_TRY_THIS_DEAL_BUY") and gc.getPlayer(self.diploScreen.getWhoTradingWith()).isNative()):
						self.addUserComment("USER_DIPLOCOMMENT_BARGAIN_BUY", -1, -1)
					elif (self.isComment(eComment, "AI_DIPLOCOMMENT_TRY_THIS_DEAL_SELL") and gc.getPlayer(self.diploScreen.getWhoTradingWith()).isNative()):
						self.addUserComment("USER_DIPLOCOMMENT_BARGAIN_SELL", -1, -1)
					#RWL Bargaining End

				# Otherwise this is a player offer to the AI
				else:

					# This is a two way deal
					if (self.diploScreen.ourOfferEmpty() == 0 and self.diploScreen.theirOfferEmpty() == 0):

						# Insert the propose trade button
						self.addUserComment("USER_DIPLOCOMMENT_PROPOSE", -1, -1)

						# During peace, see what we can get for these items
						if (not self.diploScreen.atWar()):
							if (gc.getGame().getActiveTeam() != gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()):
								self.addUserComment("USER_DIPLOCOMMENT_COMPLETE_DEAL", -1, -1)

					# Otherwise they have something on the table and we dont
					elif (self.diploScreen.theirOfferEmpty() == 0):

						# If we are at war, demand the items for peace or ask what they want
						if (self.diploScreen.atWar()):
							self.addUserComment("USER_DIPLOCOMMENT_PROPOSE", -1, -1)

						# Otherwise (during peacetime) ask what they want for our item or demand they give it to us
						else:
							if (gc.getGame().getActiveTeam() == gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()):
								self.addUserComment("USER_DIPLOCOMMENT_DEMAND_TEAM", -1, -1)

							else:
								self.addUserComment("USER_DIPLOCOMMENT_OFFER", -1, -1)

								if (gc.getPlayer(self.diploScreen.getWhoTradingWith()).AI_getAttitude(gc.getGame().getActivePlayer()) >= AttitudeTypes.ATTITUDE_PLEASED):
									self.addUserComment("USER_DIPLOCOMMENT_ASK", -1, -1)
								elif (gc.getTeam(gc.getGame().getActiveTeam()).canDeclareWar(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam())):
									self.addUserComment("USER_DIPLOCOMMENT_DEMAND", -1, -1)

					# Otherwise we have something on the table and they dont
					else:

						# If we are at war, use this item to fish for peace or propose peace with the items
						if (self.diploScreen.atWar()):
							self.addUserComment("USER_DIPLOCOMMENT_PROPOSE", -1, -1)

						# During peace, see what we can get for these items or simply gift them to the AI
						else:
							if (gc.getGame().getActiveTeam() != gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()):
								self.addUserComment("USER_DIPLOCOMMENT_FISH_FOR_DEAL", -1, -1)

							self.addUserComment("USER_DIPLOCOMMENT_GIFT", -1, -1)

			# Exit option
			self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
			self.diploScreen.startTrade( eComment, False )

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_SOMETHING_ELSE")):
			if (gc.getTeam(gc.getGame().getActiveTeam()).canDeclareWar(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam())):
				if gc.getTeam(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()).isParentOf(gc.getGame().getActiveTeam()):
					self.addUserComment("USER_DIPLOCOMMENT_REVOLUTION", -1, -1)
				else:
					self.addUserComment("USER_DIPLOCOMMENT_WAR", -1, -1)

			self.addUserComment("USER_DIPLOCOMMENT_ATTITUDE", -1, -1)
			if (gc.getTeam(gc.getGame().getActiveTeam()).AI_shareWar(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam())):
				self.addUserComment("USER_DIPLOCOMMENT_TARGET", -1, -1)

			self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_ATTITUDE") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FURIOUS") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_ATTITUDE_PLAYER_ANNOYED") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_ATTITUDE_PLAYER_CAUTIOUS") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_ATTITUDE_PLAYER_PLEASED") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FRIENDLY")):
			for i in range(gc.getMAX_CIV_PLAYERS()):
				if (gc.getPlayer(i).isAlive()):
					if ((i != gc.getGame().getActivePlayer()) and (i != self.diploScreen.getWhoTradingWith())):
						if (gc.getTeam(gc.getGame().getActiveTeam()).isHasMet(gc.getPlayer(i).getTeam()) and gc.getTeam(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()).isHasMet(gc.getPlayer(i).getTeam())):
							self.addUserComment("USER_DIPLOCOMMENT_ATTITUDE_PLAYER", i, -1, gc.getPlayer(i).getNameKey())

			self.addUserComment("USER_DIPLOCOMMENT_SOMETHING_ELSE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)

		elif (self.isComment(eComment, "AI_DIPLOCOMMENT_TARGET")):
			for i in range(gc.getMAX_CIV_PLAYERS()):
				if (gc.getPlayer(i).isAlive()):
					if (gc.getTeam(gc.getGame().getActiveTeam()).isAtWar(gc.getPlayer(i).getTeam())):
						player = gc.getPlayer(i)
						(loopCity, iter) = player.firstCity(False)
						while(loopCity):
							if (loopCity.isRevealed(gc.getGame().getActiveTeam(), False)):
								self.addUserComment("USER_DIPLOCOMMENT_TARGET_CITY", i, loopCity.getID(), loopCity.getNameKey())
							(loopCity, iter) = player.nextCity(iter, False)

			self.addUserComment("USER_DIPLOCOMMENT_SOMETHING_ELSE", -1, -1)
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)

		# The default...
		else:
			# RaR, ray Church Favours
			if (gc.getGame().isChurchPlayer(self.diploScreen.getWhoTradingWith())):
				self.addUserComment("USER_DIPLOCOMMENT_REQUEST_CHURCH_FAVOUR", -1, -1)

			elif (gc.getPlayer(gc.getGame().getActivePlayer()).canTradeWith(self.diploScreen.getWhoTradingWith())):
				# Allow us to begin another proposal
				self.addUserComment("USER_DIPLOCOMMENT_PROPOSAL", -1, -1)

			if (self.isComment(eComment, "AI_DIPLOCOMMENT_GREETINGS") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_WORST_ENEMY") or
					self.isComment(eComment, "AI_DIPLOCOMMENT_WORST_ENEMY_TRADING")):
				# If we are at war, allow to suggest peace
				if (self.diploScreen.atWar()):
					self.addUserComment("USER_DIPLOCOMMENT_SUGGEST_PEACE", -1, -1)

			# If we have a current deal, allow us to see the deals
			if (self.diploScreen.hasAnnualDeal()):
				self.addUserComment("USER_DIPLOCOMMENT_CURRENT_DEALS", -1, -1)

			if (gc.getTeam(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam())).isParentOf(gc.getGame().getActiveTeam()):
				if (gc.getTeam(gc.getGame().getActiveTeam()).canDeclareWar(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam())):
					self.addUserComment("USER_DIPLOCOMMENT_REVOLUTION", -1, -1)

				if (not gc.getTeam(gc.getGame().getActiveTeam()).isAtWar(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam())):
					self.addUserComment("USER_DIPLOCOMMENT_BUY_UNITS", -1, -1)
					self.addUserComment("USER_DIPLOCOMMENT_KING_ASK_FOR_USED_SHIP", -1, -1) # WTP, ray Kings Used Ship - START

			# WTP, ray, Foreign Kings, buy Immigrants - START
			if (gc.getPlayer(self.diploScreen.getWhoTradingWith()).isEurope()):
				if (not gc.getTeam(gc.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()).isParentOf(gc.getGame().getActiveTeam())):
					self.addUserComment("USER_DIPLOCOMMENT_ASK_FOREIGN_KING_BUY_IMMIGRANTS", -1, -1)
			# WTP, ray, Foreign Kings, buy Immigrants - END

			else:
				self.addUserComment("USER_DIPLOCOMMENT_SOMETHING_ELSE", -1, -1)

			# Exit potential
			self.addUserComment("USER_DIPLOCOMMENT_EXIT", -1, -1)
			self.diploScreen.endTrade();

	def addUserComment(self, eComment, iData1, iData2, *args):
		" Helper for adding User Comments "
		iComment = self.getCommentID( eComment )
		self.diploScreen.addUserComment( iComment, iData1, iData2, self.getDiplomacyComment(iComment), args)

	def setAIComment (self, eComment, *args):
		" Handles the determining the AI comments"
		AIString = self.getDiplomacyComment(eComment)

		if DebugLogging:
			print "CvDiplomacy.setAIComment: %s" %(eComment,)
			if (len(args)):
				print "args", args
			AIString = "(%d) - %s" %(self.getLastResponseID(), AIString)

		self.diploScreen.setAIString(AIString, args)
		self.diploScreen.setAIComment(eComment)
		self.determineResponses(eComment, args)
		self.performHeadAction(eComment)

	def performHeadAction( self, eComment ):

		if ( eComment == self.getCommentID("AI_DIPLOCOMMENT_NO_PEACE") or
		     eComment == self.getCommentID("AI_DIPLOCOMMENT_REJECT") or
		     eComment == self.getCommentID("AI_DIPLOCOMMENT_NO_DEAL") or
		     eComment == self.getCommentID("AI_DIPLOCOMMENT_CANCEL_DEAL") or
		     eComment == self.getCommentID("AI_DIPLOCOMMENT_REJECT_ASK") or
		     eComment == self.getCommentID("AI_DIPLOCOMMENT_REJECT_DEMAND") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_DISAGREE )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_ACCEPT") or
			   eComment == self.getCommentID("AI_DIPLOCOMMENT_TRY_THIS_DEAL_BUY") or # RWL Bargain Change
			   eComment == self.getCommentID("AI_DIPLOCOMMENT_TRY_THIS_DEAL_SELL") or # RWL Bargain Change
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_TRY_THIS_DEAL") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_AGREE )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_FIRST_CONTACT") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_GREETINGS") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_WORST_ENEMY") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_OFFER_PEACE") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_OFFER_CITY") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_OFFER_DEAL") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_GIVE_HELP") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_JOIN_WAR") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_STOP_TRADING") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_ASK_FOR_HELP") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_DEMAND_TRIBUTE") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_GREETING )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FURIOUS") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_WORST_ENEMY_TRADING") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_HELP_REFUSED") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_DEMAND_REJECTED") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_JOIN_DENIED") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_STOP_DENIED") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_FURIOUS )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_ANNOYED") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_ANNOYED )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_CAUTIOUS") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_CAUTIOUS )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_PLEASED") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_SORRY") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_PLEASED )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FRIENDLY") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_GLAD") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_THANKS") ):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_FRIENDLY )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_KISS_PINKY") or
			   eComment == self.getCommentID("AI_DIPLOCOMMENT_KING_ASK_FOR_GOLD") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_BUY_UNITS") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_KING_OFFERS_USED_SHIP") or # WTP, ray Kings Used Ship - START
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_FOREIGN_KING_OFFERS_IMMIGRANTS") or # WTP, ray, Foreign Kings, buy Immigrants - START
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_KING_GIFT_SHIP") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_KING_REVIVE")):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_OFFER_PINKY )
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_ACCEPT_PINKY") or 
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_KING_ACCEPT_GOLD")):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_PINKY_ACCEPT )
			CyInterface().playGeneralSound("AS2D_KISS_MY_RING")
		elif ( eComment == self.getCommentID("AI_DIPLOCOMMENT_REJECT_PINKY") or
		       eComment == self.getCommentID("AI_DIPLOCOMMENT_KING_REFUSE_GOLD")):
			self.diploScreen.performHeadAction( LeaderheadAction.LEADERANIM_PINKY_REJECT )

		return

	def getDiplomacyComment (self, eComment):
		"Function to get the user String"
		debugString = "CvDiplomacy.getDiplomacyComment: %s" %(eComment,)
		eComment = int(eComment)
		if DebugLogging:
			print debugString, eComment

		szString = ""
		szFailString = "Error***: No string found for eComment: %s"

		if ( gc.getDiplomacyInfo(eComment) ):
			DiplomacyTextInfo = gc.getDiplomacyInfo(eComment)
			if ( not DiplomacyTextInfo ):
				print "%s IS AN INVALID DIPLOCOMMENT" %(eComment,)
				CvUtil.pyAssert(True, "CvDiplomacy.getDiplomacyComment: %s does not have a DiplomacyTextInfo" %(eComment,))
				return szFailString %(eComment,)

			szString = self.filterUserResponse(DiplomacyTextInfo)

		else:
			szString = szFailString %(eComment,)

		return szString

	def setLastResponseID(self, iResponse):
		self.iLastResponseID = iResponse

	def getLastResponseID(self):
		return self.iLastResponseID

	def isUsed(self, var, i, num):
		"returns True if any element in the var list is True"
		for j in range(num):
			if (var(i, j)):
				return True
		return False

	def filterUserResponse(self, diploInfo):
		"pick the user's response from a CvDiplomacyTextInfo, based on response conditions"
		if (self.diploScreen.getWhoTradingWith() == -1):
			return ""

		theirPlayer = gc.getPlayer(self.diploScreen.getWhoTradingWith())
		ourPlayer = gc.getActivePlayer()
		responses = []

		for i in range(diploInfo.getNumResponses()):

			# check attitude of other player towards me
			if (self.isUsed(diploInfo.getAttitudeTypes, i, AttitudeTypes.NUM_ATTITUDE_TYPES)):
				att = theirPlayer.AI_getAttitude(CyGame().getActivePlayer())
				if (not diploInfo.getAttitudeTypes(i, att)):
					continue

			# check civ type
			if (self.isUsed(diploInfo.getCivilizationTypes, i, gc.getNumCivilizationInfos()) and
				not diploInfo.getCivilizationTypes(i, theirPlayer.getCivilizationType())):
				continue

			# check leader type
			if (self.isUsed(diploInfo.getLeaderHeadTypes, i, gc.getNumLeaderHeadInfos()) and
				not diploInfo.getLeaderHeadTypes(i, theirPlayer.getLeaderType())):
				continue

			# check power type
			if (self.isUsed(diploInfo.getDiplomacyPowerTypes, i, DiplomacyPowerTypes.NUM_DIPLOMACYPOWER_TYPES)):
				theirPower = theirPlayer.getPower()
				ourPower = ourPlayer.getPower()

				if (ourPower < (theirPower / 2)):
					if not diploInfo.getDiplomacyPowerTypes(i, DiplomacyPowerTypes.DIPLOMACYPOWER_STRONGER):
						continue

				elif (ourPower > (theirPower * 2)):
					if not diploInfo.getDiplomacyPowerTypes(i, DiplomacyPowerTypes.DIPLOMACYPOWER_WEAKER):
						continue

				else:
					if not diploInfo.getDiplomacyPowerTypes(i, DiplomacyPowerTypes.DIPLOMACYPOWER_EQUAL):
						continue

			# passed all tests, so add to response list
			for j in range(diploInfo.getNumDiplomacyText(i)):
				responses.append(diploInfo.getDiplomacyText(i, j))

		# pick a random response
		numResponses = len(responses)
		if (numResponses>0):
			iResponse = gc.getASyncRand().get(numResponses, "Python Diplomacy ASYNC")
			self.setLastResponseID(iResponse)
			return responses[iResponse]

		return ""	# no responses matched

	def handleUserResponse(self, eComment, iData1, iData2):
		if DebugLogging:
			print "CvDiplomacy.handleUserResponse: %s" %(eComment,)

		diploScreen = CyDiplomacy()

		# If we accept peace
		if (self.isComment(eComment, "USER_DIPLOCOMMENT_PEACE")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_PEACE"))

		# If we choose war
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_WAR") or self.isComment(eComment, "USER_DIPLOCOMMENT_REVOLUTION")):
			diploScreen.declareWar()

		# If we are asking for units
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_BUY_UNITS")):
			args = diploScreen.militaryHelp()
			iNumUnits = args[0]
			eUnit = args[1]
			eProfession = args[2]
			iPrice = args[3]
			if (iNumUnits > 0 and iPrice <= gc.getPlayer(gc.getGame().getActivePlayer()).getGold()):
				if eProfession == -1:
					szName = gc.getUnitInfo(eUnit).getTextKey()
				else:
					szName = gc.getProfessionInfo(eProfession).getTextKey()
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_BUY_UNITS"), iPrice, szName)
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_REJECT_BUY_UNIS"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACCEPT_BUY_UNITS")):
			CyInterface().playGeneralSound("AS2D_KISS_MY_RING")
			diploScreen.buyUnits()
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KING_GIFT_SHIP")):
			CyInterface().playGeneralSound("AS2D_KISS_MY_RING")
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KING_REVIVE")):
			CyInterface().playGeneralSound("AS2D_KISS_MY_RING")
			diploScreen.closeScreen()

		# If we wish to make a trade proposal or try to renegotiate
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_PROPOSAL") or
					self.isComment(eComment, "USER_DIPLOCOMMENT_RENEGOTIATE")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_TRADING"))
			diploScreen.showAllTrade(True)
		
		# RaR, ray Church Favours
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REQUEST_CHURCH_FAVOUR")):
			iPrice = gc.getPlayer(gc.getGame().getActivePlayer()).getChurchFavourPrice()
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_CHURCH_FAVOUR"), iPrice)

		# WTP, ray Kings Used Ship - START
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KING_ASK_FOR_USED_SHIP")):
			iRandomUsedShip = gc.getPlayer(gc.getGame().getActivePlayer()).getRandomUsedShipClassTypeID()
			if (iRandomUsedShip != -1):
				iPrice = gc.getPlayer(gc.getGame().getActivePlayer()).getUsedShipPrice(iRandomUsedShip)
				if (gc.getPlayer(gc.getGame().getActivePlayer()).isKingWillingToTradeUsedShips() and gc.getPlayer(gc.getGame().getActivePlayer()).getGold() >= iPrice):
					gc.getPlayer(gc.getGame().getActivePlayer()).resetCounterForUsedShipDeals()
					szName = gc.getUnitClassInfo(iRandomUsedShip).getTextKey()
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_KING_OFFERS_USED_SHIP"), iPrice, iRandomUsedShip, szName)
				else:
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_KING_REFUSES_TO_OFFER_USED_SHIP"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_KING_REFUSES_TO_OFFER_USED_SHIP"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACCEPT_BUY_USED_SHIP")):
			CyInterface().playGeneralSound("AS2D_KISS_MY_RING")
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_USED_SHIPS, iData2, iData1)
			diploScreen.closeScreen()
		# WTP, ray Kings Used Ship - END

		# WTP, ray, Foreign Kings, buy Immigrants - START
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ASK_FOREIGN_KING_BUY_IMMIGRANTS")):
			iRandomImmigrant = gc.getPlayer(gc.getGame().getActivePlayer()).getRandomForeignImmigrantClassTypeID(self.diploScreen.getWhoTradingWith())
			if (iRandomImmigrant != -1):
				iPrice = gc.getPlayer(gc.getGame().getActivePlayer()).getForeignImmigrantPrice(iRandomImmigrant, self.diploScreen.getWhoTradingWith())
				if (gc.getPlayer(gc.getGame().getActivePlayer()).isForeignKingWillingToTradeImmigrants(self.diploScreen.getWhoTradingWith()) and gc.getPlayer(gc.getGame().getActivePlayer()).getGold() >= iPrice):
					gc.getPlayer(self.diploScreen.getWhoTradingWith()).resetCounterForForeignImmigrantsDeals()
					szName = gc.getUnitClassInfo(iRandomImmigrant).getTextKey()
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_FOREIGN_KING_OFFERS_IMMIGRANTS"), iPrice, iRandomImmigrant, szName)
				else:
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_FOREIGN_KING_REFUSES_IMMIGRANTS"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_FOREIGN_KING_REFUSES_IMMIGRANTS"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACCEPT_FOREIGN_IMMIGRANTS")):
			CyInterface().playGeneralSound("AS2D_KISS_MY_RING")
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_FOREIGN_IMMIGRANTS, iData2, iData1)
			diploScreen.closeScreen()
		# WTP, ray, Foreign Kings, buy Immigrants - END

		# If we want to propose a trade
		elif(self.isComment(eComment, "USER_DIPLOCOMMENT_PROPOSE")):
			if (diploScreen.offerDeal() == 1):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ACCEPT"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_REJECT"))

		# If we ask for peace
		elif(self.isComment(eComment, "USER_DIPLOCOMMENT_SUGGEST_PEACE")):
			if (diploScreen.offerDeal() == 1):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_PEACE"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_NO_PEACE"))

		# If we accept a trade
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACCEPT")):
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_GLAD"))

		# If we reject a trade
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REJECT")):
			diploScreen.setAIOffer(0)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_SORRY"))

		#RWL Bargaining	 Changed Start
		# If we offer a deal, or is we are fishing for a deal, or if we are offering peace or fishing for peace
		elif (#self.isComment(eComment, "USER_DIPLOCOMMENT_OFFER") or
					self.isComment(eComment, "USER_DIPLOCOMMENT_COMPLETE_DEAL") or
					#self.isComment(eComment, "USER_DIPLOCOMMENT_FISH_FOR_DEAL") or
					self.isComment(eComment, "USER_DIPLOCOMMENT_OFFER_PEACE")):
			if (diploScreen.counterPropose() == 1):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_TRY_THIS_DEAL"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_NO_DEAL"))
		#RWL Bargaining	 Changed End

		#RWL Bargaining Start
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_OFFER")):
			if (diploScreen.counterPropose() == 1):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_TRY_THIS_DEAL_BUY"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_NO_DEAL"))
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_FISH_FOR_DEAL")):
			if (diploScreen.counterPropose() == 1):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_TRY_THIS_DEAL_SELL"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_NO_DEAL"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_BARGAIN_SELL")):
		
			if (gc.getPlayer(gc.getGame().getActivePlayer()).tryGetNewBargainPriceSell()):
				if (diploScreen.counterPropose() == 1):
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_BARGAIN_SELL_SUCCESS"))
				else:	
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_BARGAIN_SELL_SUCCESS"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_BARGAIN_FAIL"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_BARGAIN_BUY")):
		
			if (gc.getPlayer(gc.getGame().getActivePlayer()).tryGetNewBargainPriceBuy()):
				if (diploScreen.counterPropose() == 1):
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_BARGAIN_BUY_SUCCESS"))
				else:	
					self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_BARGAIN_BUY_SUCCESS"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_BARGAIN_FAIL"))
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_BARGAIN_ACCEPT")):
			if (self.diploScreen.ourOfferEmpty() == 1 or self.diploScreen.theirOfferEmpty() == 1):
				diploScreen.setAIOffer(0)
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_NO_DEAL"))
			else:
				diploScreen.implementDeal()
				diploScreen.setAIOffer(0)
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_GLAD"))
		#RWL Bargaining	 End
				
		# if we are asking for something
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ASK")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ASK_HELP, -1, -1)
			if (diploScreen.offerDeal()):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ACCEPT_ASK"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_REJECT_ASK"))

		# if we are demanding something
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_DEMAND")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_MADE_DEMAND, -1, -1)
			if (diploScreen.offerDeal()):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ACCEPT_DEMAND"))
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_REJECT_DEMAND"))

		# if we are demanding something from our teammate
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_DEMAND_TEAM")):
			diploScreen.offerDeal()
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ACCEPT_DEMAND_TEAM"))

		# If we are giving a gift
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_GIFT")):
			diploScreen.offerDeal()
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_THANKS"))

		# If we decide to view current deals
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_CURRENT_DEALS")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_CURRENT_DEALS"))
			diploScreen.showAllTrade(False)

		# If we give help
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_GIVE_HELP")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_GIVE_HELP, -1, -1)
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_THANKS"))

		# If we accept their demand
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACCEPT_DEMAND")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACCEPT_DEMAND, -1, -1)
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_THANKS"))

		# If we accept the offer
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACCEPT_OFFER")):
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to help
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REFUSE_HELP")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REFUSED_HELP, -1, -1)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_HELP_REFUSED"))

		# If we reject their demand
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REJECT_DEMAND")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REJECTED_DEMAND, -1, -1)

			if (gc.getPlayer(self.diploScreen.getWhoTradingWith()).AI_demandRebukedWar(gc.getGame().getActivePlayer())):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_DECLARE_WAR"))
				diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_DEMAND_WAR, -1, -1)
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_DEMAND_REJECTED"))

		# If we join their war
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_JOIN_WAR")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_JOIN_WAR, diploScreen.getData(), -1)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to join their war
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_NO_JOIN_WAR")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NO_JOIN_WAR, -1, -1)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_JOIN_DENIED"))

		# If we stop the trading
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_STOP_TRADING")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_STOP_TRADING, diploScreen.getData(), -1)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to stop the trading
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_NO_STOP_TRADING")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NO_STOP_TRADING, -1, -1)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_STOP_DENIED"))

		# If we want to go back to first screen
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_NEVERMIND")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_WELL"))

		# If we want to discuss something else
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_SOMETHING_ELSE")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_SOMETHING_ELSE"))

		# If we want to ask them to what their attitude is
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ATTITUDE")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE"))

		# If we want to ask them to what their attitude is on a specific player
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ATTITUDE_PLAYER")):
			eAttitude = gc.getPlayer(self.diploScreen.getWhoTradingWith()).AI_getAttitude(iData1)

			if (eAttitude == AttitudeTypes.ATTITUDE_FURIOUS):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FURIOUS"), gc.getPlayer(iData1).getNameKey())
			elif (eAttitude == AttitudeTypes.ATTITUDE_ANNOYED):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_ANNOYED"), gc.getPlayer(iData1).getNameKey())
			elif (eAttitude == AttitudeTypes.ATTITUDE_CAUTIOUS):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_CAUTIOUS"), gc.getPlayer(iData1).getNameKey())
			elif (eAttitude == AttitudeTypes.ATTITUDE_PLEASED):
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_PLEASED"), gc.getPlayer(iData1).getNameKey())
			else:
				self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FRIENDLY"), gc.getPlayer(iData1).getNameKey())

		# If we want to ask them to change their target
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_TARGET")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_TARGET"))

		# If we want to ask them to change their target to a specific city
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_TARGET_CITY")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_TARGET_CITY, iData1, iData2)
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_TARGET_CITY"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KISS_PINKY")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_ACCEPT_PINKY"), gc.getPlayer(gc.getGame().getActivePlayer()).getTaxRate() + diploScreen.getData())

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACCEPT_PINKY")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACCEPT_TAX_RATE, diploScreen.getData(), -1)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_TAX_PARTY")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_REJECT_PINKY"), gc.getYieldInfo(iData1).getTextKey())

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REJECT_PINKY")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REFUSE_TAX_RATE, iData1, iData2)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KING_GIVE_GOLD")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_KING_ACCEPT_GOLD"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KING_ACCEPTED_GOLD")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACCEPT_KING_GOLD, diploScreen.getData(), -1)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KING_REFUSE_GOLD")):
			self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_KING_REFUSE_GOLD"))

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_KING_REFUSED_GOLD")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REFUSE_KING_GOLD, diploScreen.getData(), -1)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_LIVE_AMONG_NATIVES_ACCEPT")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_LIVE_AMONG_NATIVES, diploScreen.getData(), -1)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_TREASURE_TRANSPORT_ACCEPT")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_TRANSPORT_TREASURE, diploScreen.getData(), -1)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_FOUND_CITY_NO_FOOD_YES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_FOUND_CITY_CHECK_NATIVES, diploScreen.getData(), -1)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_FOUND_CITY_INLAND_YES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_FOUND_CITY_CHECK_NATIVES, diploScreen.getData(), -1)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_FOUND_FIRST_CITY")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_FOUND_CITY, diploScreen.getData(), False)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_FOUND_CITY_BUY")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_FOUND_CITY, diploScreen.getData(), True)
			diploScreen.closeScreen()

		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_FOUND_CITY_TAKE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_FOUND_CITY, diploScreen.getData(), False)
			diploScreen.closeScreen()
		
		# ray18
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_CITY_BUY")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_CITY, diploScreen.getData(), True)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_CITY_TAKE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_CITY, diploScreen.getData(), False)
			diploScreen.closeScreen()
		# Ende ray18
		
		# TAC Native Mercs 
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_MERC")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_MERC, diploScreen.getData(), False)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_MERC_CHEAP")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_MERC, diploScreen.getData(), True)
			diploScreen.closeScreen()
		# Ende TAC Native Mercs 
		
		# R&R, ray, Native Slave, START 
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_NATIVE_SLAVE, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# R&R, ray, Native Slave, END 
		
		# R&R, ray, African Slaves, START 
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_AFRICAN_SLAVES, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# R&R, ray, African Slaves, END 
		
		# RaR, ray Prisons Crowded
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_PRISONS")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_PRISONERS, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray Prisons Crowded
		
		# RaR, ray Revolutionary Noble
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_NOBLE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_NOBLE, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray Revolutionary Noble
		
		# RaR, ray Bishop
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_BISHOP")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_BISHOP, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray Bishop
		
		# RaR, ray Church
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_PAY_CHURCH_DEMAND")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_PAY_CHURCH_DEMAND, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REFUSE_CHURCH_DEMAND")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REFUSE_CHURCH_DEMAND, -1, -1)
			diploScreen.closeScreen()
		# RaR, ray Church
		
		# RaR, ray Church Favours
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_CHURCH_FAVOUR_KING")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CHURCH_FAVOUR, 1, -1)
			diploScreen.closeScreen()
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_CHURCH_FAVOUR_EUROPEAN")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CHURCH_FAVOUR, 2, iData1)
			diploScreen.closeScreen()
		#elif (self.isComment(eComment, "USER_DIPLOCOMMENT_CHURCH_FAVOUR_FOUNDING_FATHERS")):
			#diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CHURCH_FAVOUR, 3, -1)
			#diploScreen.closeScreen()
		# RaR, ray Church Favours
		
		# RaR, ray, Smuggling
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_SMUGGLERS")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_SMUGGLERS, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray, Smuggling
		
		# RaR, ray, Pirates
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_PIRATES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_PIRATES, diploScreen.getData(), -1)
			diploScreen.closeScreen()
			
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_BRIBE_PIRATES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_BRIBE_PIRATES, diploScreen.getData(), -1)
			diploScreen.closeScreen()
			
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_NOT_ACQUIRE_PIRATESS")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CREATE_ENEMY_PIRATES, -1, -1)
			diploScreen.closeScreen()
		# RaR, ray, Pirates
		
		# RaR, ray, Continental Guard
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_CONTINENTAL_GUARD")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_CONTINENTAL_GUARD, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray, Continental Guard
		
		# RaR, ray, Mortar
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_MORTAR")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_MORTAR, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray, Mortar
		
		# RaR, ray, Rangers
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_RANGERS")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_RANGERS, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray, Rangers
		
		# RaR, ray, Conquistadors
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ACQUIRE_CONQUISTADORS")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACQUIRE_CONQUISTADORS, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray, Conquistadors
		
		# TAC Revolution Support
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REV_NEED_LAND")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REV_SUPPORT, True, False)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_REV_NEED_SEA")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REV_SUPPORT, False, True)
			diploScreen.closeScreen()
		# Ende Revolution Support
		
		# TAC European Wars
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_EUROPE_WAR_YES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_EUROPE_WAR, diploScreen.getData(), 1)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_EUROPE_WAR_YES_BUT")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_EUROPE_WAR, diploScreen.getData(), 2)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_EUROPE_WAR_NO")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_EUROPE_WAR, diploScreen.getData(), 3)
			diploScreen.closeScreen()
		# TAC European Wars
		
		# RaR, ray Stealing Immigrants
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_STEALING_IMMIGRANT_YES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_STEALING_IMMIGRANT, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# RaR, ray Stealing Immigrants
		
		# RWL European Peace
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_EUROPE_PEACE_YES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_EUROPE_PEACE, diploScreen.getData(), 1)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_EUROPE_PEACE_NO")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_EUROPE_PEACE, diploScreen.getData(), 2)
			diploScreen.closeScreen()
		# RWL European Peace
		
		# WTP, ray, Royal Intervention, START
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ROYAL_INTERVENTION_YES_GOLD")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ROYAL_INTERVENTION, diploScreen.getData(), 1)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ROYAL_INTERVENTION_YES_TAX_INCREASE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ROYAL_INTERVENTION, diploScreen.getData(), 2)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_ROYAL_INTERVENTION_NO")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ROYAL_INTERVENTION, diploScreen.getData(), 3)
			diploScreen.closeScreen()
		# WTP, ray, Royal Intervention, END
		
		# WTP, ray, Privateers DLL Diplo Event - START
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_PRIVATEERS_ADMIT")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_PRIVATEERS_ACCUSATION, diploScreen.getData(), 1)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_PRIVATEERS_DENY")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_PRIVATEERS_ACCUSATION, diploScreen.getData(), 2)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_PRIVATEERS_WAR")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_PRIVATEERS_ACCUSATION, diploScreen.getData(), 3)
			diploScreen.closeScreen()
		# WTP, ray, Privateers DLL Diplo Event - END
		
		# RaR, ray Church Wars
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_CHURCH_WAR_YES")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CHURCH_WAR, diploScreen.getData(), 1)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_CHURCH_WAR_YES_BUT")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CHURCH_WAR, diploScreen.getData(), 2)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_CHURCH_WAR_NO")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CHURCH_WAR, diploScreen.getData(), 3)
			diploScreen.closeScreen()
		# RaR, ray Church Wars
		
		# WTP, ray, Colonial Intervention In Native War - START
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_COLONIAL_INTERVENTION_NATIVE_WAR_END")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_COLONIAL_INTERVENTION_NATIVE_WAR, diploScreen.getData(), 1)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_COLONIAL_INTERVENTION_NATIVE_WAR_CONTINUE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_COLONIAL_INTERVENTION_NATIVE_WAR, diploScreen.getData(), 2)
			diploScreen.closeScreen()
		# WTP, ray, Colonial Intervention In Native War - END
		
		# WTP, ray, Big Colonies and Native Allies War - START
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_COLONIES_AND_NATIVE_ALLIES_WAR_REFUSE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_COLONIES_AND_NATIVE_ALLIES_WAR_REFUSE, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_COLONIES_AND_NATIVE_ALLIES_WAR_ACCEPT")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_COLONIES_AND_NATIVE_ALLIES_WAR_ACCEPT, diploScreen.getData(), -1)
			diploScreen.closeScreen()
		# WTP, ray, Big Colonies and Native Allies War - END
		
		# R&R, ray, Natives Trading
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_NATIVES_TRADE_ACCEPT")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NATIVE_TRADE, diploScreen.getData(), 1)
			diploScreen.closeScreen()
			
		elif (self.isComment(eComment, "USER_DIPLOCOMMENT_NATIVES_TRADE_DECLINE")):
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NATIVE_TRADE, diploScreen.getData(), 2)
			diploScreen.closeScreen()
		# R&R, ray, Natives Trading

		else:
			diploScreen.closeScreen()

	def dealCanceled( self ):

		self.setAIComment(self.getCommentID("AI_DIPLOCOMMENT_TRADING"))

		return

	def isComment(self, eComment, strComment):
		'bool - comment matching'
		if ( gc.getDiplomacyInfo(eComment).getType() == strComment ):
			return True
		return False

	def getCommentID(self, strComment):
		'int - ID for DiploCommentType'
		for i in range(gc.getNumDiplomacyInfos()):
			if ( gc.getDiplomacyInfo(i).getType() == strComment ):
				return i

		print "Jason " + strComment
		return -1
