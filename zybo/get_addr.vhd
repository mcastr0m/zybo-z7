library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity get_addr is
    generic(
        COOR_W : integer := CONST_COOR_W - 1;
        ADDR_W : integer := CONST_ADDR_W - 1
    );
    port(
        x    : in std_logic_vector(COOR_W downto 0);
        y    : in std_logic_vector(COOR_W downto 0);
        addr : out std_logic_vector(ADDR_W downto 0)
    );
end entity;

architecture arquitectura of get_addr is
    signal value : std_logic_vector(17 downto 0);
begin
    -- Aquí genera una advertencia, es debido a que de los 18 bits, solo se toman 16 que
    -- es lo que importa. Se crea de 20 bits debido a que la operación puede generar una salida
    -- de ese tamano.
    addr <= value(ADDR_W downto 0);

    process(x, y) is 
    begin
        value <= std_logic_vector((unsigned(y) * unsigned(RES_X)) + unsigned(x));
    end process;
end arquitectura;