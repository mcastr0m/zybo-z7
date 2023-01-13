library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity addr_generator is
    generic(
        COOR_W : integer := CONST_COOR_W - 1;
        ADDR_W : integer := CONST_ADDR_W - 1
    );
    port(
        clk        : in std_logic;
        rst        : in std_logic;
        adv        : in std_logic;
        vs         : out std_logic;
        column     : out std_logic_vector(COOR_W downto 0);
        row        : out std_logic_vector(COOR_W downto 0);
        addr_left  : out std_logic_vector(ADDR_W downto 0);
        addr_right : out std_logic_vector(ADDR_W downto 0);
        addr_out   : out std_logic_vector(ADDR_W downto 0)
    );
end entity;

architecture arquitectura of addr_generator is

    component get_addr is
        port(
            x    : in std_logic_vector(COOR_W downto 0);
            y    : in std_logic_vector(COOR_W downto 0);
            addr : out std_logic_vector(ADDR_W downto 0)
        );
    end component;
    
    -- Regstros que hacen de contadores para las coordenandas. 
    signal x : std_logic_vector(COOR_W downto 0) := (others => '0');
    signal y : std_logic_vector(COOR_W downto 0) := (others => '0');
    signal c : std_logic_vector(COOR_W downto 0) := (others => '0');
    signal r : std_logic_vector(COOR_W downto 0) := (others => '0');
    signal s : std_logic_vector(COOR_W downto 0) := (others => '0');
    signal p : std_logic_vector(COOR_w downto 0) := (others => '0');
    
    -- Senales de sincronia, que hacen el control de filas y columnas de la
    -- imagen. Se inicializa la HS en 1 para que cuando se inicie el proceso,
    -- tomar en cuenta que se tiene que hacer una lectura completa de las 
    -- primeras ventanas.
    signal this_vs : std_logic := '0';
begin
    vs <= this_vs;
    row <= r;
    column <= c;
    
    s <= std_logic_vector(unsigned(y) + unsigned(r));
    p <= std_logic_vector(unsigned(x) + unsigned(c));
    
    get_addr_left : get_addr
    port map(
        x    => p,
        y    => s,
        addr => addr_left
    );
    
    get_addr_right: get_addr
    port map(
        x    => p,
        y    => s,
        addr => addr_right
    );
    
    get_addr_out : get_addr
    port map(
        x    => x,
        y    => y,
        addr => addr_out
    );
    
    -- Proceso de interacion, en el que actualiza los registro para hacer los
    -- incrementos de los contadores.
    process(clk, rst)
    begin
        if rst = '1' then
            this_vs <= '0';
            x <= (others => '0');
            y <= (others => '0');
            c <= (others => '0');
            r <= (others => '0');
        elsif rising_edge(clk) then
            if adv = '1' then
                if r = MAX_I then
                    if c = MAX_W then
                        if x = MAX_X then
                            if y = MAX_Y then
                                this_vs <= '1';
                            else
                                x <= (others => '0');
                                r <= (others => '0');
                                c <= (others => '0');
                                y <= std_logic_vector(unsigned(y) + 1);
                            end if;
                        else
                            r <= (others => '0');
                            c <= (others => '0');
                            x <= std_logic_vector(unsigned(x) + 1);
                        end if;
                    else
                        r <= (others => '0');
                        c <= std_logic_vector(unsigned(c) + 1);
                    end if;
                else
                    r <= std_logic_vector(unsigned(r) + 1);
                end if;
            else 
                x <= x;
                y <= y;
                c <= c;
                r <= r;
                this_vs <= this_vs;
            end if;
        end if;
    end process;
    
end arquitectura;