library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity image_memory is
    generic(
        ADDR_W : integer := CONST_ADDR_W - 1;
        DATA_W : integer := CONST_DATA_W - 1
    );
    port(
        clk   : in std_logic;
        wea   : in std_logic;
        ena   : in std_logic;
        waddr : in std_logic_vector(ADDR_W downto 0);
        raddr : in std_logic_vector(ADDR_W downto 0);
        din   : in std_logic_vector(DATA_W downto 0);
        dout  : out std_logic_vector(DATA_W downto 0)
    );
end entity;

architecture arquitectura of image_memory is
    signal vector : IMAGE;
begin

    -- Proceso para el almacenado de la informacion de la imagen en la 
    -- memoria.
    process(clk) 
    begin
        if rising_edge(clk) then
            if wea = '1' then
                vector(to_integer(unsigned(waddr))) <= din;
            end if;
        end if;
    end process;

    -- Proceso para la lectura de la informacion de la imagen almacenda
    -- en la memoria.
    process(clk) 
    begin
        if rising_edge(clk) then
            if ena = '1' then
                dout <= vector(to_integer(unsigned(raddr)));
            end if;
        end if;
    end process;
end arquitectura;