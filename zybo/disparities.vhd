library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.constants.all;

entity disparities is
    generic(
        COST_W : integer := CONST_COST_W - 1;
        DATA_W : integer := CONST_DATA_W - 1
    );
    port(
        cost  : in COSTES;
        level : out std_logic_vector(DATA_W downto 0)
    );
end disparities;

architecture arquitectura of disparities is
    -- Definicion de los buffers temporales donde se alamacenan las salidas de los
    -- comparadores.
    type A_CTS_0 is array(0 to (D / 2) - 1) of std_logic_vector(COST_W downto 0);
    type A_CTS_1 is array(0 to (D / 4) - 1) of std_logic_vector(COST_W downto 0);
    type A_CTS_2 is array(0 to (D / 8) - 1) of std_logic_vector(COST_W downto 0);
    type A_CTS_3 is array(0 to (D / 16) - 1) of std_logic_vector(COST_W downto 0);
    type A_CTS_4 is array(0 to (D / 32) - 1) of std_logic_vector(COST_W downto 0);
    
    type A_IDX_0 is array(0 to (D / 2) - 1) of std_logic_vector(DATA_W downto 0);
    type A_IDX_1 is array(0 to (D / 4) - 1) of std_logic_vector(DATA_W downto 0);
    type A_IDX_2 is array(0 to (D / 8) - 1) of std_logic_vector(DATA_W downto 0);
    type A_IDX_3 is array(0 to (D / 16) - 1) of std_logic_vector(DATA_W downto 0);
    type A_IDX_4 is array(0 to (D / 32) - 1) of std_logic_vector(DATA_W downto 0);
    
    -- Buffers que contienen las salidas de los comparadores.
    signal array_cost_level_0 : A_CTS_0;
    signal array_cost_level_1 : A_CTS_1;
    signal array_cost_level_2 : A_CTS_2;
    signal array_cost_level_3 : A_CTS_3;
    signal array_cost_level_4 : A_CTS_4;
    signal cost_level_5       : std_logic_vector(COST_W downto 0);
    
    -- Buffers que contienen los indices de las salidas de los
    -- comparadores.
    signal array_index_level_0 : A_IDX_0;
    signal array_index_level_1 : A_IDX_1;
    signal array_index_level_2 : A_IDX_2;
    signal array_index_level_3 : A_IDX_3;
    signal array_index_level_4 : A_IDX_4;
    signal index_level_5       : std_logic_vector(DATA_W downto 0);
    
    component comparator is
        port(
            data_a     : in std_logic_vector(COST_W downto 0);
            data_b     : in std_logic_vector(COST_W downto 0);
            index_a    : in std_logic_vector(DATA_W downto 0);
            index_b    : in std_logic_vector(DATA_W downto 0);
            data_min   : out std_logic_vector(COST_W downto 0);
            index_min  : out std_logic_vector(DATA_W downto 0)
        );
    end component;
begin

    comparator_level_0_for : for i in 0 to (D / 2) - 1 generate
        comparator_level_0_inst : comparator
        port map(
            data_a    => cost(2 * i),
            data_b    => cost(2 * i + 1),
            index_a   => std_logic_vector(to_unsigned(2 * i, CONST_DATA_W)),
            index_b   => std_logic_vector(to_unsigned(2 * i + 1, CONST_DATA_W)),
            data_min  => array_cost_level_0(i),
            index_min => array_index_level_0(i)
        );
    end generate;
    
    comparator_level_1_for : for i in 0 to (D / 4) - 1 generate
        comparator_level_1_inst : comparator
        port map(
            data_a    => array_cost_level_0(2 * i),
            data_b    => array_cost_level_0(2 * i + 1),
            index_a   => array_index_level_0(2 * i),
            index_b   => array_index_level_0(2 * i + 1),
            data_min  => array_cost_level_1(i),
            index_min => array_index_level_1(i)
        );
    end generate;
    
    comparator_level_2_for : for i in 0 to (D / 8) - 1 generate
        comparator_level_2_inst : comparator
        port map(
            data_a    => array_cost_level_1(2 * i),
            data_b    => array_cost_level_1(2 * i + 1),
            index_a   => array_index_level_1(2 * i),
            index_b   => array_index_level_1(2 * i + 1),
            data_min  => array_cost_level_2(i),
            index_min => array_index_level_2(i)
        );
    end generate;
    
    comparator_level_3_for : for i in 0 to (D / 16) - 1 generate
        comparator_level_3_inst : comparator
        port map(
            data_a    => array_cost_level_2(2 * i),
            data_b    => array_cost_level_2(2 * i + 1),
            index_a   => array_index_level_2(2 * i),
            index_b   => array_index_level_2(2 * i + 1),
            data_min  => array_cost_level_3(i),
            index_min => array_index_level_3(i)
        );
    end generate;
    
    comparator_level_4_for : for i in 0 to (D / 32) - 1 generate
        comparator_level_4_inst : comparator
        port map(
            data_a    => array_cost_level_3(2 * i),
            data_b    => array_cost_level_3(2 * i + 1),
            index_a   => array_index_level_3(2 * i),
            index_b   => array_index_level_3(2 * i + 1),
            data_min  => array_cost_level_4(i),
            index_min => array_index_level_4(i)
        );
    end generate;
    
    comparator_level_5_inst : comparator
    port map(
        data_a    => array_cost_level_4(0),
        data_b    => array_cost_level_4(1),
        index_a   => array_index_level_4(0),
        index_b   => array_index_level_4(1),
        data_min  => cost_level_5,
        index_min => index_level_5
    );

    level <= index_level_5;

end arquitectura;

