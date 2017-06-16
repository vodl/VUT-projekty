-- ------------------------------ --
-- ledc8x8.vhd                    --
-- Vojtech Dlapal                 --  
-- xdlapa01@stud.fit.vutbr.cz     -- 
-- 30.10.2011                     --
-- ------------------------------ -- 

library IEEE;
use ieee.std_logic_1164.all; -- knihovna obsahujici zakladni syntetizovatelne funkce
use ieee.std_logic_arith.all; --  podpora pro syntezu aritmetickych operaci
use ieee.std_logic_unsigned.all; -- umoznuje pracovat bezznamenkove

-- entita podle zadani
entity ledc8x8 is
  port (
        SMCLK : in std_logic;
        RESET : in std_logic;
        ROW   : out std_logic_vector(7 downto 0);
        LED   : out std_logic_vector(0 to 7) -- obraceno kvuli obraceni
                                        -- a prehlednosti vystupu z dec	  
       );
  end ledc8x8;


architecture behavioral of ledc8x8 is

  signal ce, switch: std_logic;
  signal tmpr: std_logic_vector(7 downto 0) := "11111110"; -- vnitrni signal pro radky
  signal tmpl: std_logic_vector(7 downto 0) := "00000000"; -- vnitrni signal pro led

begin
           
-- 22 bitovy citac      
ctrl_cnt: process (SMCLK, RESET)   
   variable cnt_in : std_logic_vector(21 downto 0) := (others => '0');
begin	
		
		if RESET = '1' then
          cnt_in := (others => '0');
      elsif SMCLK'event and (SMCLK = '1') then
          cnt_in := cnt_in + 1;
      end if;
		
		ce <= '0';
		
		if cnt_in(7 downto 0) = X"FF" then  
		    ce <= '1';
		end if;
		
		switch <= cnt_in(21);
		
end process ctrl_cnt;


-- 8 bitovy rotacni registr s povolovacim vstupem ce taktovany SMCLK
row_cnt: process (SMCLK, RESET)

begin
		if RESET = '1' then
          tmpr <= "11111110";
      elsif SMCLK'event and (SMCLK = '1') then
		    if (ce = '1') then
			     tmpr <= tmpr(6 downto 0) & tmpr(7);
			 end if;
      end if;
end process row_cnt;


ROW <= tmpr;


-- dekoder - prideluje radku sestavu diod
dec: process (tmpr)

begin

	tmpl<= X"00";
	case tmpr is
	  when X"FE" => tmpl <= "10001000"; --r0
	  when X"FD" => tmpl <= "01010000"; --r1
	  when X"FB" => tmpl <= "01010000"; --r2
	  when X"F7" => tmpl <= "00100000"; --r3
	  when X"EF" => tmpl <= "00001110"; --r4
	  when X"DF" => tmpl <= "00001001"; --r5
	  when X"BF" => tmpl <= "00001001"; --r6
	  when X"7F" => tmpl <= "00001110"; --r7
								  --01234567
	  when others => null;
   end case;

end process dec;


-- multiplexor - na zaklade switch rozsvedcuje sestavy diod na radku nebo ne
mux: process(switch, tmpl)

begin

  case switch is
    when '1' => LED <= "00000000";
    when '0' => LED <= tmpl;
    when others => null;
  end case;
end process mux;      
      
        
end architecture;