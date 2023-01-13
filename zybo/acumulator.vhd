library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity acumulator is
    generic(
        COST_W : integer := CONST_COST_W - 1
    );
    port(
        clk      : in std_logic;
        rst      : in std_logic;
        adv      : in std_logic;
        cost_in  : in std_logic_vector(COST_W downto 0);
        cost_out : out std_logic_vector(COST_W downto 0)
    );
end entity;

architecture arquitectura of acumulator is
    signal this_acum : std_logic_vector(COST_W downto 0) := (others => '0');
begin

    cost_out <= this_acum;

    acumulation_process : process(clk, rst)
    begin
        if rst = '1' then
            this_acum <= (others => '0');
        elsif rising_edge(clk) then
            if adv = '1' then
                this_acum <= std_logic_vector(unsigned(this_acum) + unsigned(cost_in));
            end if;
        end if;
    end process acumulation_process;
end arquitectura;