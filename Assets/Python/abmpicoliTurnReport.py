from CvPythonExtensions import *
import datetime

gc = CyGlobalContext()
    
def report():
    terrainCoast = TerrainTypes.TERRAIN_COAST
    terrainShallowCoast = TerrainTypes.TERRAIN_SHALLOW_COAST
    final_result=[]
    player=gc.getPlayer(0)
    europe = gc.getPlayer(player.getParent())
    game=gc.getGame()
    map=gc.getMap()
    turn = game.getGameTurn()
    prefix="abmpicolireport-"+str(game.getGameTurn())+"-"+(str(datetime.datetime.now()).replace(' ','-'))
    final_result.append(prefix)
    print prefix+"game turn report: turn=" +  CyGameTextMgr().getTimeStr(turn, False) + "(" + str(turn)+") : player=" + str(player.getName())
    line="team\tcity\tx\ty\tdomain\tarea\tdistance\tvisited\ttrade_value\tgold_needed\tnative_demand\tnative_demand_buy_price\tnative_treasure\tproducts"
    print prefix+"\t"+line
    final_result.append(line)
    units=[]
    (pUnit,iter) = player.firstUnit()
    while pUnit:
        if pUnit.cargoSpace() > 0:
            units.append(pUnit)
        (pUnit,iter) = player.nextUnit(iter)

    if len(units) == 0:
        print prefix+"\tabmpicoli no suitable units for cargo"
    
    for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
            ePlayer = gc.getPlayer(iLoopPlayer)
            player_gold=ePlayer.getGold();
            #if (player.isAlive() and player.isNative() and (gc.getTeam(player.getTeam()).isHasMet(activePlayer.getTeam()))):
            if ((ePlayer.isAlive() and ePlayer.isNative())) or ePlayer.getID() == 0:
                
                (pLoopCity, iter) = ePlayer.firstCity(False)
                while(pLoopCity):
                    native_demand_yield=gc.getYieldInfo(pLoopCity.AI_getDesiredYield())
                    native_demand_name="N/A"
                    native_demand_buy_price=0
                    if native_demand_yield:
                        native_demand_name=native_demand_yield.getDescription()
                        native_demand_buy_price=native_demand_yield.getNativeBuyPrice()
                    cityplot = pLoopCity.plot()
                    position=(cityplot.getX(),cityplot.getY())
                    visited = pLoopCity.isScoutVisited(player.getTeam())
                    owned = ePlayer.getID()==0
                    if visited:
                        visited="visited"
                    else:
                        visited="not visited"
                    
                    domain="LAND"
                    if pLoopCity.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN()):
                        
                        domain="COAST.0RIVERLAKE"
                        
                        for dx in range(-1,2):
                            if domain=="COAST.2DEEP":
                                break
                            for dy in range(-1,2):
                                dplot = map.plot(position[0]+dx,position[1]+dy)
                                if dplot:
                                    if dplot.getTerrainType() == terrainShallowCoast:
                                        domain="COAST.1SHALLOW"
                                        continue
                                    if dplot.getTerrainType() == terrainCoast:
                                        domain="COAST.2DEEP"
                                        break
                    dist=99999
                    for pUnit in units:
                        if pUnit.canMoveInto(cityplot,False,False,False):
                            new_dist=pUnit.getPathTurns(cityplot,0,False)
                            if new_dist >=0 :
                                dist=min(new_dist,dist)
                    dist=dist*1.0
                    value=0
                    gold_reserve=0
                    net_yield=0
                    products=""
                    comma=""
                    if owned:
                        # check for warehouse capacity and add as an extra value in the priority.
                        # code collected from WarehouseAdvisor.py
                        iMaxYield = pLoopCity.getMaxYieldCapacity()
                        iProducedYield=0
                        storage_used=0
                        for iYield in range(YieldTypes.NUM_YIELD_TYPES):
                            the_yield=gc.getYieldInfo(iYield)
                            if not the_yield.isCargo():
                                continue
                            if the_yield.isIgnoredForStorageCapacity():
                                continue
                            iProducedYield += pLoopCity.calculateActualYieldProduced(iYield)
                            storage_used += pLoopCity.getYieldStored(iYield)
                        turns_to_overflow= max(1,(iMaxYield - storage_used) / max(0.001,iProducedYield))
                        value += 1000.0/turns_to_overflow
                    for iYield in range(YieldTypes.NUM_YIELD_TYPES):
                        the_yield=gc.getYieldInfo(iYield)
                        if not the_yield.isCargo():
                            continue
                        buy_price= europe.getYieldBuyPrice(iYield)
                        sell_price = europe.getYieldSellPrice(iYield)
                        the_price_value=(buy_price+sell_price)/2.0
                        native_sell_price = the_yield.getNativeSellPrice()
                        stored = pLoopCity.getYieldStored(iYield)
                        if stored == 0 :
                            continue
                        if ePlayer.isNative() and the_yield.getNativeSellPrice() > 0 :
                            gold_reserve += native_sell_price * stored
                            value_per_item=the_price_value*1.0 - native_sell_price / 2.0 
                            value += stored * value_per_item
                            products = products + comma + str(the_yield.getDescription())+"("+str(stored)+"; profit="+str(value_per_item)+")"
                            comma=","
                        elif owned:
                            value += stored * sell_price*1.0
                    line=ePlayer.getCivilizationShortDescription(0)+\
                        "\t"+pLoopCity.getName()+\
                        "\t"+str(position[0])+\
                        "\t"+str(position[1])+\
                        "\t"+domain+\
                        "\t"+str(cityplot.getArea())+\
                        "\t"+(str(dist).replace(".",","))+\
                        "\t"+str(visited)+\
                        "\t"+str(value/(dist+0.5)).replace(".",",")+\
                        "\t"+str(gold_reserve).replace(".",",")+\
                        "\t"+native_demand_name+\
                        "\t"+str(native_demand_buy_price)+\
                        "\t"+str(player_gold)+\
                        "\t"+products
                    print prefix+"\t"+line
                    final_result.append(line)
                    (pLoopCity, iter) = ePlayer.nextCity(iter, False)
    return final_result