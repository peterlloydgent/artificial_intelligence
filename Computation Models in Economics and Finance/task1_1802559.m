csv_file_table = readtable('AAL.csv');
column = 2;
total = 0;
average = 0;
row_top = 1;
row_bottom = 7;
window_size = 7;
rows = height(csv_file_table)-6;
investment = 1000000;
initial_investment = 1000000;
investment_step = 100000;
list_of_buying_prices = [];
profit = 0;
format long
for frame = 1:rows
    % sum over the window to get the total to be used for average
    for row = row_top:row_bottom 
        table = csv_file_table(row,column);
        total = total + table.('Var2');
    end
    %calculate present average to be compared with previous average
    tmp_average = total/window_size
    current_price = getCurrentPrice(row_bottom,column,csv_file_table);
    bos = buyorsell(tmp_average,average)
    
    switch bos
        case 'buy'
            %buy - add to cell price bought at with default step of £100k
            % investment
            if(investment>investment_step)
                list_of_buying_prices = [list_of_buying_prices,current_price];
                investment = investment - investment_step
            else
                disp("cannot afford to invest any more")
            end
        case 'sell'
            %sell - compare revenue from buying to selling and update
            %investment
            if(~isempty(list_of_buying_prices))
                % work out net change in price and then multiply investment
                % step in order to work out the real change in pounds.
                % iterate through a list of prices
                for k=1:length(list_of_buying_prices)
                    change_in_value = (((current_price - list_of_buying_prices(k))/list_of_buying_prices(k))*investment_step);
                    investment = investment + (investment_step + change_in_value);
                    fprintf('change_in_value %.4f \n', change_in_value);
                    fprintf('current bank account: %.4f \n',investment);
                    profit = profit + change_in_value;
                    fprintf('profit: %.4f ', profit)
                end
                % at end of sale setlist of buying prices to 0
                list_of_buying_prices = [];
            else
                disp("cannot sell as nothing is invested ") 
            end   
    end

    % at end of buy or sell evaluation set the new average to tmp_average
    % for comparison and reset total. Increment window with row_top+1 and
    % row_bottom+1
    average = tmp_average;
    total = 0;
    row_top = row_top+1;
    row_bottom = row_bottom+1;
end

% get and return the current price.
function [return_price] = getCurrentPrice(row_bottom,column,csv_file_table)
    table = csv_file_table(row_bottom,column);
    return_price =  table.('Var2');
end

% decide whether to buy or  sell given new average larger than average.
function [buy_or_sell] = buyorsell(tmp_average,average)
    if(tmp_average>average)
       buy_or_sell = "buy";
    else
       buy_or_sell = "sell";
    end
end


