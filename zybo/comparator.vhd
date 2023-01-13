library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity comparator is
    generic(
        COST_W : integer := CONST_COST_W - 1;
        DATA_W : integer := CONST_DATA_W - 1
    );
    port(
        data_a     : in std_logic_vector(COST_W downto 0);
        data_b     : in std_logic_vector(COST_W downto 0);
        index_a    : in std_logic_vector(DATA_W downto 0);
        index_b    : in std_logic_vector(DATA_W downto 0);
        data_min   : out std_logic_vector(COST_W downto 0);
        index_min  : out std_logic_vector(DATA_W downto 0)
    );
end entity;

architecture arquitectura of comparator is
begin
    process(data_a, data_b, index_a, index_b)
    begin
        if(data_a < data_b) then
            data_min <= data_a;
            index_min <= index_a;
        else
            data_min <= data_b;
            index_min <= index_b;
        end if;
    end process;
end arquitectura;