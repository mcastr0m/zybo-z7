library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity processor_buffer is
    generic(
        COOR_W : integer := CONST_COOR_W - 1;
        COST_W : integer := CONST_COST_W - 1;
        DATA_W : integer := CONST_DATA_W - 1
    );
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
end entity;

architecture arquitectura of processor_buffer is

    -- Componente sad.vhd que permite aplicar la funcion SAD entre dos entradas de 8 bits, y producir una
    -- salida de 8 bits. Es enteramente combinacional.
    component sad is
        port(
            left  : in std_logic_vector(DATA_W downto 0);
            right : in std_logic_vector(DATA_W downto 0);
            cost  : out std_logic_vector(DATA_W downto 0)
        );
    end component;
    
    component acumulator is
        port(
            clk      : in std_logic;
            rst      : in std_logic;
            adv      : in std_logic;
            cost_in  : in std_logic_vector(COST_W downto 0);
            cost_out : out std_logic_vector(COST_W downto 0)
        );
    end component;
    
    component flip_flop is
        port(
            clk  : in std_logic;
            set  : in std_logic;
            rst  : in std_logic;
            din  : in std_logic_vector(COST_W downto 0);
            dout : out std_logic_vector(COST_W downto 0)
        );
    end component;

    -- Definicion el buffer que permita ir almacenando los valores temporales obtenidos mediante el operador 
    -- SAD.
    type TEMP_SAD is array(0 to WW - 1) of std_logic_vector(DATA_W downto 0);

    -- Crear los buffers que van a contener los datos para el procesado. Se crear filas de acuerdo al tamano
    -- de la ventana que se definio, es decir 9. Se usan renglones ya que esto permitir?a hacer los recorridos.
    signal row0_left  : std_logic_vector(23 downto 0) := (others => '0');
    signal row1_left  : std_logic_vector(23 downto 0) := (others => '0');
    signal row2_left  : std_logic_vector(23 downto 0) := (others => '0');
    signal row0_right : std_logic_vector(23 downto 0) := (others => '0');
    signal row1_right : std_logic_vector(23 downto 0) := (others => '0');
    signal row2_right : std_logic_vector(23 downto 0) := (others => '0');

    -- Se crea un registro con el arreglo definido anteriormente, que va a almacenar todos los resultados de
    -- aplicar el operador SAD. Este arreglo es de tamano definido en la cosntante WW, es decir 9 elementos.
    signal buffer_temp_sad : TEMP_SAD := (others => (others => '0'));
    
    signal counter  : std_logic_vector(COOR_W downto 0) := (others => '0');
    signal state    : std_logic_vector(1 downto 0) := b"11";
    signal activate : std_logic_vector(2 downto 0) := (others => '0');
    
    signal in_acumulator : std_logic_vector(COST_W downto 0) := (others => '0');
    signal out_acumulator : std_logic_vector(COST_W downto 0) := (others => '0');

begin
    -- Operador SAD usado para buscar el resultado de entre todos los elementos de todas las filas, se est?n
    -- usando ambas filas (tanto la izquierda como la derecha). el resultado se almacena en un buffer temporal    
    -- que es usado para hacer ls sumas mas adelante.
    sad_0_for : for i in W - 1 downto 0 generate
        sad_0_inst : sad
        port map(
            left  => row0_left(((i + 1) * 8) - 1 downto (i * 8)),
            right => row0_right(((i + 1) * 8) - 1 downto (i * 8)),
            cost  => buffer_temp_sad(i)
        );
    end generate;

    sad_1_for : for i in W - 1 downto 0 generate
        sad_1_inst : sad
        port map(
            left  => row1_left(((i + 1) * 8) - 1 downto (i * 8)),
            right => row1_right(((i + 1) * 8) - 1 downto (i * 8)),
            cost  => buffer_temp_sad(i + 3)
        );
    end generate;

    sad_2_for : for i in W - 1 downto 0 generate
        sad_2_inst : sad
        port map(
            left  => row2_left(((i + 1) * 8) - 1 downto (i * 8)),
            right => row2_right(((i + 1) * 8) - 1 downto (i * 8)),
            cost  => buffer_temp_sad(i + 6)
        );
    end generate;
    
    acumulator_inst : acumulator
    port map(
        clk      => clk,
        rst      => activate(2),
        adv      => activate(1),
        cost_in  => in_acumulator,
        cost_out => out_acumulator
    );
    
    flip_flop_inst : flip_flop
    port map(
        clk  => clk,
        set  => activate(0),
        rst  => rst,
        din  => out_acumulator,
        dout => cost
    );

    -- El proceso de escritura consiste en recibir los datos y, mientras se est?n almacenando y haciendo los 
    -- recorridos para que el siguiente dato se almacen al final.
    process_write_left : process(clk)
    begin        
        if rising_edge(clk) then
            if wrena_l = '1' and state = b"00" then
                case row is
                    when b"000000000" => 
                        row0_left <= row0_left(15 downto 0) & left;
                    when b"000000001" =>
                        row1_left <= row1_left(15 downto 0) & left;
                    when b"000000010" =>
                        row2_left <= row2_left(15 downto 0) & left;
                    when others =>
                        row0_left <= (others => '0');
                        row1_left <= (others => '0');
                        row2_left <= (others => '0');
                end case;
            end if;
        end if;
    end process process_write_left;
    
    process_write_right : process(clk)
    begin          
        if rising_edge(clk) then
            if wrena_r = '1' and state = b"00" then
                case row is
                    when b"000000000" =>
                        row0_right <= row0_right(15 downto 0) & right;
                    when b"000000001" =>
                        row1_right <= row1_right(15 downto 0) & right;
                    when b"000000010" =>
                        row2_right <= row2_right(15 downto 0) & right;
                    when others =>
                        row0_right <= (others => '0');
                        row1_right <= (others => '0');
                        row2_right <= (others => '0');
                end case;
            end if; 
            end if;
    end process process_write_right;
    
    -- Desde que es necesario implementar un contador, se tiene que controlar el proceso 
    -- de acumulacion por medio de contadores. Hay dos estados: cuando se esta escribiendo 
    -- una nueva columna y cuando se tiene que hacer la acumulacion despues de que se 
    -- escribio una columna.
    counter_process : process(clk, rst)
    begin
        if rst = '1' then
            counter <= (others => '0');
            state <= b"11";
        elsif rising_edge(clk) then
            if adv = '1' then
                if state = b"11" then
                    state <= "00";
                    counter <= (others => '0');
                elsif state = b"00" then
                    if counter = MAX_V then
                        state <= b"01";
                        counter <= (others => '0');
                    else
                        state <= b"00";
                        counter <= std_logic_vector(unsigned(counter) + 1);
                    end if;
                elsif state = b"01" then
                    if counter = MAX_I then
                        state <= b"10";
                        counter <= (others => '0');
                    else
                        state <= b"01";
                        counter <= std_logic_vector(unsigned(counter) + 1);
                    end if;
                elsif state = b"10" then
                    if counter = MAX_K then
                        state <= b"00";
                        counter <= (others => '0');
                    else
                        state <= b"10";
                        counter <= std_logic_vector(unsigned(counter) + 1);
                    end if;
                else
                    state <= b"11";
                    counter <= (others => '0');
                end if;
            else
                state <= b"11";
                counter <= (others => '0');
            end if;
        end if;
    end process counter_process;
    
    activate_process : process(state)
    begin
       case state is
            when b"00" => activate <= b"001"; process_wait <= '1';
            when b"01" => activate <= b"100"; process_wait <= '0';
            when b"10" => activate <= b"010"; process_wait <= '0';
            when others => activate <= b"000"; process_wait <= '0';
        end case;
    end process activate_process;
    
    acumulate_process : process(counter, buffer_temp_sad)
    begin
        case counter is 
            when b"000000000" => in_acumulator <= b"0000" & buffer_temp_sad(0);
            when b"000000001" => in_acumulator <= b"0000" & buffer_temp_sad(1);
            when b"000000010" => in_acumulator <= b"0000" & buffer_temp_sad(2);
            when b"000000011" => in_acumulator <= b"0000" & buffer_temp_sad(3);
            when b"000000100" => in_acumulator <= b"0000" & buffer_temp_sad(4);
            when b"000000101" => in_acumulator <= b"0000" & buffer_temp_sad(5);
            when b"000000110" => in_acumulator <= b"0000" & buffer_temp_sad(6);
            when b"000000111" => in_acumulator <= b"0000" & buffer_temp_sad(7);
            when b"000001000" => in_acumulator <= b"0000" & buffer_temp_sad(8);
            when others => in_acumulator <= (others => '0');
        end case;
    end process acumulate_process;
    
end arquitectura;
