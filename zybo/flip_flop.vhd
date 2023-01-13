library ieee;
use ieee.std_logic_1164.all;
use work.constants.all;

entity flip_flop is
    generic(
        COST_W : integer := CONST_COST_W - 1
    );
    port(
        clk  : in std_logic;
        set  : in std_logic;
        rst  : in std_logic;
        din  : in std_logic_vector(COST_W downto 0);
        dout : out std_logic_vector(COST_W downto 0)
    );
end entity;

architecture arquitectura of flip_flop is
begin
    process(clk, set, rst)
    begin 
        if rst = '1' then
            dout <= (others => '0');
        elsif rising_edge(clk) then
            if set /= rst then
                dout <= din;
            elsif set = '1' and rst = '1' then
                dout <= (others => 'Z');
            end if;
        end if;
    end process;

end arquitectura;
