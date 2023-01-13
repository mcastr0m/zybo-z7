-- Contiene los parametros de las imagenes, tales como resolucion
-- vertical y horizontal, asi como su tamano total.

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package constants is
    -- Constantes
    constant SIZE : integer := 39360;  -- Tamano total de la imagen (anchura por altura).
    constant D    : integer := 64;     -- Niveles de disparidad.
    constant W    : integer := 3;      -- Tamano de la ventana.
    constant WW   : integer := 9;      -- Numero total de elemento de la ventana.
    
    -- Anchos de bits
    constant CONST_DATA_W : integer := 8;   -- Ancho de bits de datos.
    constant CONST_ADDR_W : integer := 16;  -- Ancho de las direcciones de memoria.
    constant CONST_COST_W : integer := 12;  -- Ancho maximo del acumulador.
    constant CONST_COOR_W : integer := 9;   -- Ancho para las coordenadas.
    constant CONST_INDX_W : integer := 8;   -- Ancho de bits de un indice.
    
    -- Comparadores
    constant RES_X : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"101000000";  -- 320.
    constant RES_Y : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"001111011";  -- 123.
    constant MAX_X : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"100111100";  -- 316.
    constant MAX_Y : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"001110111";  -- 119.
    constant MAX_W : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"001000001";  -- 65.
    constant MAX_V : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"011000101";  -- 197
    constant MAX_I : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"000000010";  -- 2.
    constant MAX_K : std_logic_vector(CONST_COOR_W - 1 downto 0) := b"000001000";  -- 8.
    
    -- Vectores
    type PIXELES is array(0 to D - 1) of std_logic_vector(CONST_DATA_W - 1 downto 0);
    type IMAGE is array(0 to SIZE - 1) of std_logic_vector(CONST_DATA_W - 1 downto 0);
    type COSTES is array(0 to D - 1) of std_logic_vector(CONST_COST_W - 1 downto 0);
end package constants;

-- Paginas de soporte
-- https://support.xilinx.com/s/question/0D52E00006hplltSAA/synth-86859-multi-driven-net-on-pin-output-error?language=en_US
-- https://support.xilinx.com/s/article/75527?language=en_US
-- https://0xstubs.org/measuring-time-in-a-bare-metal-zynq-application/