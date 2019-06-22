ctrs = [1,0.5]

main()
function main()
    bid1 = input('advertiser 1 please give bid: ' )
    private_value_firm1 = input('advertiser 1 please give private value: ' )
    bid2 = input('advertiser 1 please give bid: ' )
    private_value_firm2 = input('advertiser 1 please give private value: ' )
    firm1_pays = 0
    ctr_firm1 = 0
    firm2_pays = 0
    ctr_firm2 = 0
    if(bid1>bid2)
        %if highest bidder then pay the second price. next bidder pays 0
        firm1_pays = bid2
        ctr_firm1 = ctrs[0]
        firm2_pays = 0
        ctr_firm1 = ctrs[1]
    elseif(bid2<bid1)
        firm2_pays = bid2
        firm1_pays = 0
        ctr_firm2 = ctrs[0]
        ctr_firm1 = ctrs[1]
        %firm 2 pays the lower bidders price. next bidder pays 0
    end
    print("payoff for firm 1 " + (private_value_firm1-ctr_firm1))
    print("payoff for firm 2 " + (private_value_firm2-ctr_firm2))
end

