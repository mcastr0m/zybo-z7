library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity match_control is
    generic(
        ADDR_W : integer := CONST_ADDR_W - 1;
        DATA_W : integer := CONST_DATA_W - 1
    );
    port(
        clk      : in std_logic;
        rst      : in std_logic;
        adv      : in std_logic;
        wea      : in std_logic;
        ena      : in std_logic;
        finished : out std_logic;
        addr     : in std_logic_vector(ADDR_W downto 0);
        left     : in std_logic_vector(DATA_W downto 0);
        right    : in std_logic_vector(DATA_W downto 0);
        deph     : out std_logic_vector(DATA_W downto 0)
    );
end entity;

architecture arquitectura of match_control is

    component main_processor is
    port(
        clk         : in std_logic;
        rst         : in std_logic;
        adv         : in std_logic;
        finished    : out std_logic;
        addr_left   : out std_logic_vector(ADDR_W downto 0);
        addr_right  : out std_logic_vector(ADDR_W downto 0);
        addr_out    : out std_logic_vector(ADDR_W downto 0);
        pixel_left  : in std_logic_vector(DATA_W downto 0);
        pixel_right : in std_logic_vector(DATA_W downto 0);
        pixel_out   : out std_logic_vector(DATA_W downto 0);
        wea         : out std_logic;
        ena         : out std_logic
    );
    end component;

    component image_memory is
        port(
            clk   : in std_logic;
            wea   : in std_logic;
            ena   : in std_logic;
            waddr : in std_logic_vector(ADDR_W downto 0);
            raddr : in std_logic_vector(ADDR_W downto 0);
            din   : in std_logic_vector(DATA_W downto 0);
            dout  : out std_logic_vector(DATA_W downto 0)
        );
    end component;

    signal this_pixel_left  : std_logic_vector(DATA_W downto 0);
    signal this_pixel_right : std_logic_vector(DATA_W downto 0);
    signal this_pixel_out   : std_logic_vector(DATA_W downto 0);
    signal this_addr_left   : std_logic_vector(ADDR_W downto 0);
    signal this_addr_right  : std_logic_vector(ADDR_W downto 0);
    signal this_addr_out    : std_logic_vector(ADDR_W downto 0);
    signal this_wea         : std_logic;
    signal this_ena         : std_logic;
begin

    image_memory_left_inst : image_memory
    port map(
        clk   => clk,
        wea   => wea,
        ena   => this_ena,
        waddr => addr,
        raddr => this_addr_left,
        din   => left,
        dout  => this_pixel_left
    );

    image_memory_right_inst : image_memory
    port map(
        clk   => clk,
        wea   => wea,
        ena   => this_ena,
        waddr => addr,
        raddr => this_addr_right,
        din   => right,
        dout  => this_pixel_right
    );

    image_memory_deph_inst : image_memory
    port map(
        clk   => clk,
        wea   => this_wea,
        ena   => ena,
        waddr => this_addr_out,
        raddr => addr,
        din   => this_pixel_out,
        dout  => deph
    );

    main_processor_inst : main_processor
    port map(
        clk         => clk,
        rst         => rst,
        adv         => adv,
        finished    => finished,
        addr_left   => this_addr_left,
        addr_right  => this_addr_right,
        addr_out    => this_addr_out,
        pixel_left  => this_pixel_left,
        pixel_right => this_pixel_right,
        pixel_out   => this_pixel_out,
        wea         => this_wea,
        ena         => this_ena
    );
end arquitectura;