library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity main_processor is
    generic(
        COST_W : integer := CONST_COST_W - 1;
        DATA_W : integer := CONST_DATA_W - 1;
        COOR_W : integer := CONST_COOR_W - 1;
        ADDR_W : integer := CONST_ADDR_W - 1
    );
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
end entity;

architecture arquitectura of main_processor is
    component processor_buffer is
        port(
            clk          : in std_logic;
            rst          : in std_logic;
            adv          : in std_logic;
            wrena_l      : in std_logic;
            wrena_r      : in std_logic;
            left         : in std_logic_vector(DATA_W downto 0);
            right        : in std_logic_vector(DATA_W downto 0);
            row          : in std_logic_vector(COOR_W downto 0);
            cost         : out std_logic_vector(COST_W downto 0);
            process_wait : out std_logic
        );
    end component;

    component addr_generator is
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
    end component;

    component disparities is
        port(
            cost  : in COSTES;
            level : out std_logic_vector(DATA_W downto 0)
        );
    end component;

    -- Definicion de el tipo arreglo para La transpferencia de los datos entre los
    -- bufferes.
    type ARRAY_ASSIGN is array(0 to D - 1) of std_logic_vector(DATA_W downto 0);

    -- Declaración de los registros que van a contener los datos que se van a 
    -- transferir entre los bufferes.
    signal assign : ARRAY_ASSIGN := (others => (others => '0')); 

    -- Decraración de los registros que se usan para hacer el control de el procesado.
    signal enable_counter : std_logic := '0';
    signal enable_write_l : std_logic := '0';
    signal enable_write_r : std_logic_vector(D - 1 downto 0) := (others => '0');
    signal process_wait   : std_logic_vector(D - 1 downto 0) := (others => '0');
    signal cost_out       : COSTES;
    signal x_column       : std_logic_vector(COOR_W downto 0);
    signal y_row          : std_logic_vector(COOR_W downto 0);
    signal vs             : std_logic;

begin
    enable_counter <= adv and process_wait(0);
    finished <= vs;
    ena <= not vs;
    wea <= not vs;

    addr_generator_inst : addr_generator
    port map(
        clk        => clk,
        rst        => rst,
        adv        => enable_counter,
        vs         => vs,
        column     => x_column,
        row        => y_row,
        addr_left  => addr_left,
        addr_right => addr_right,
        addr_out   => addr_out
    );

    processor_buffer_for : for i in 0 to D - 1 generate
        processor_buffer_insta : processor_buffer
        port map(
            clk          => clk,
            rst          => rst,
            adv          => adv,
            wrena_l      => enable_write_l,
            wrena_r      => enable_write_r(i),
            left         => pixel_left,
            right        => assign(i),
            row          => y_row,
            cost         => cost_out(i),
            process_wait => process_wait(i)
        );
    end generate;

    disparities_inst : disparities
    port map(
        cost  => cost_out,
        level => pixel_out
    );

    assign_process : process(pixel_left, pixel_right)
    begin
        case(x_column) is
            when b"000000000" =>
                enable_write_l <= '1';
                enable_write_r <= x"0000000000000001";
                assign(0) <= pixel_right;
            when b"000000001" =>
                enable_write_l <= '1';
                enable_write_r <= x"0000000000000003";
                for i in 0 to 1 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000000010" =>
                enable_write_l <= '1';
                enable_write_r <= x"0000000000000007";
                for i in 0 to 2 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000000011" =>
                enable_write_l <= '0';
                enable_write_r <= x"000000000000000E";
                for i in 1 to 3 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000000100" =>
                enable_write_l <= '0';
                enable_write_r <= x"000000000000001C";
                for i in 2 to 4 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000000101" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000000038";
                for i in 3 to 5 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000000110" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000000070";
                for i in 4 to 6 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000000111" =>
                enable_write_l <= '0';
                enable_write_r <= x"00000000000000E0";
                for i in 5 to 7 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001000" =>
                enable_write_l <= '0';
                enable_write_r <= x"00000000000001C0";
                for i in 6 to 8 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001001" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000000380";
                for i in 7 to 9 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001010" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000000700";
                for i in 8 to 10 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001011" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000000E00";
                for i in 9 to 11 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001100" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000001C00";
                for i in 10 to 12 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001101" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000003800";
                for i in 11 to 13 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001110" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000007000";
                for i in 12 to 14 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000001111" =>
                enable_write_l <= '0';
                enable_write_r <= x"000000000000E000";
                for i in 13 to 15 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010000" =>
                enable_write_l <= '0';
                enable_write_r <= x"000000000001C000";
                for i in 14 to 16 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010001" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000038000";
                for i in 15 to 17 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010010" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000070000";
                for i in 16 to 18 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010011" =>
                enable_write_l <= '0';
                enable_write_r <= x"00000000000E0000";
                for i in 17 to 19 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010100" =>
                enable_write_l <= '0';
                enable_write_r <= x"00000000001C0000";
                for i in 18 to 20 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010101" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000380000";
                for i in 19 to 21 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010110" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000700000";
                for i in 20 to 22 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000010111" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000E00000";
                for i in 21 to 23 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011000" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000001C00000";
                for i in 22 to 24 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011001" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000003800000";
                for i in 23 to 25 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011010" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000007000000";
                for i in 24 to 26 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011011" =>
                enable_write_l <= '0';
                enable_write_r <= x"000000000E000000";
                for i in 25 to 27 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011100" =>
                enable_write_l <= '0';
                enable_write_r <= x"000000001C000000";
                for i in 26 to 28 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011101" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000038000000";
                for i in 27 to 29 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011110" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000070000000";
                for i in 28 to 30 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000011111" =>
                enable_write_l <= '0';
                enable_write_r <= x"00000000E0000000";
                for i in 29 to 31 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100000" =>
                enable_write_l <= '0';
                enable_write_r <= x"00000001C0000000";
                for i in 30 to 32 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100001" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000380000000";
                for i in 31 to 33 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100010" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000700000000";
                for i in 32 to 34 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100011" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000E00000000";
                for i in 33 to 35 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100100" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000001C00000000";
                for i in 34 to 36 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100101" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000380000000";
                for i in 35 to 37 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100110" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000007000000000";
                for i in 36 to 38 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000100111" =>
                enable_write_l <= '0';
                enable_write_r <= x"000000E000000000";
                for i in 37 to 39 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101000" =>
                enable_write_l <= '0';
                enable_write_r <= x"000001C000000000";
                for i in 38 to 40 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101001" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000038000000000";
                for i in 39 to 41 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101010" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000070000000000";
                for i in 40 to 42 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101011" =>
                enable_write_l <= '0';
                enable_write_r <= x"00000E0000000000";
                for i in 41 to 43 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101100" =>
                enable_write_l <= '0';
                enable_write_r <= x"00001C0000000000";
                for i in 42 to 44 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101101" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000380000000000";
                for i in 43 to 45 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101110" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000700000000000";
                for i in 44 to 46 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000101111" =>
                enable_write_l <= '0';
                enable_write_r <= x"0000E00000000000";
                for i in 45 to 47 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110000" =>
                enable_write_l <= '0';
                enable_write_r <= x"0001C00000000000";
                for i in 46 to 48 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110001" =>
                enable_write_l <= '0';
                enable_write_r <= x"0003800000000000";
                for i in 47 to 49 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110010" =>
                enable_write_l <= '0';
                enable_write_r <= x"0007000000000000";
                for i in 48 to 50 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110011" =>
                enable_write_l <= '0';
                enable_write_r <= x"000E000000000000";
                for i in 49 to 51 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110100" =>
                enable_write_l <= '0';
                enable_write_r <= x"001C000000000000";
                for i in 50 to 52 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110101" =>
                enable_write_l <= '0';
                enable_write_r <= x"0038000000000000";
                for i in 51 to 53 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110110" =>
                enable_write_l <= '0';
                enable_write_r <= x"0070000000000000";
                for i in 52 to 54 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000110111" =>
                enable_write_l <= '0';
                enable_write_r <= x"00E0000000000000";
                for i in 53 to 55 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111000" =>
                enable_write_l <= '0';
                enable_write_r <= x"01C0000000000000";
                for i in 54 to 56 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111001" =>
                enable_write_l <= '0';
                enable_write_r <= x"0380000000000000";
                for i in 55 to 57 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111010" =>
                enable_write_l <= '0';
                enable_write_r <= x"0700000000000000";
                for i in 56 to 58 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111011" =>
                enable_write_l <= '0';
                enable_write_r <= x"0E00000000000000";
                for i in 57 to 59 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111100" =>
                enable_write_l <= '0';
                enable_write_r <= x"1C00000000000000";
                for i in 58 to 60 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111101" =>
                enable_write_l <= '0';
                enable_write_r <= x"3800000000000000";
                for i in 59 to 61 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111110" =>
                enable_write_l <= '0';
                enable_write_r <= x"7000000000000000";
                for i in 60 to 62 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"000111111" =>
                enable_write_l <= '0';
                enable_write_r <= x"E000000000000000";
                for i in 61 to 63 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"001000000" =>
                enable_write_l <= '0';
                enable_write_r <= x"C000000000000000";
                for i in 62 to 63 loop
                    assign(i) <= pixel_right;
                end loop;
            when b"001000001" =>
                enable_write_l <= '0';
                enable_write_r <= x"8000000000000000";
                assign(63) <= pixel_right;
            when others =>
                enable_write_l <= '0';
                enable_write_r <= x"0000000000000000";
                for i in 0 to D - 1 loop
                    assign(i) <= (others => '0');
                end loop;
        end case;
    end process assign_process; 

end arquitectura;