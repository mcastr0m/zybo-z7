library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity sad is
    generic(
        DATA_W : integer := CONST_DATA_W - 1
    );
    port(
        left  : in std_logic_vector(DATA_W downto 0);
        right : in std_logic_vector(DATA_W downto 0);
        cost  : out std_logic_vector(DATA_W downto 0)
    );
end entity;

architecture arquitectura of sad is
begin
    sad_process : process(left, right)
    begin
        cost <= std_logic_vector(abs(signed(left) - signed(right)));
    end process;
end arquitectura;
