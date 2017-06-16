-- phpMyAdmin SQL Dump
-- version 3.4.5
-- http://www.phpmyadmin.net
--
-- Počítač: localhost
-- Vygenerováno: Ned 09. pro 2012, 16:45
-- Verze MySQL: 5.5.28
-- Verze PHP: 5.3.19

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Databáze: `xdlapa01`
--

-- --------------------------------------------------------

--
-- Struktura tabulky `doplaci`
--

CREATE TABLE IF NOT EXISTS `doplaci` (
  `lek_kod` int(10) NOT NULL,
  `pojistovna_cislo` int(10) NOT NULL,
  `castka` int(10) NOT NULL,
  KEY `lek_kod` (`lek_kod`),
  KEY `pojistovna_cislo` (`pojistovna_cislo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Vypisuji data pro tabulku `doplaci`
--

INSERT INTO `doplaci` (`lek_kod`, `pojistovna_cislo`, `castka`) VALUES
(1093, 111, 50),
(1093, 205, 50),
(1093, 211, 50),
(1093, 201, 50),
(3378, 111, 40),
(3378, 205, 18),
(3378, 211, 42),
(3378, 201, 35),
(1066, 111, 47),
(1066, 205, 78),
(1066, 211, 62),
(1066, 201, 85);

-- --------------------------------------------------------

--
-- Struktura tabulky `lek`
--

CREATE TABLE IF NOT EXISTS `lek` (
  `kod` int(10) NOT NULL,
  `nazev` varchar(100) NOT NULL,
  `na_predpis` tinyint(1) NOT NULL,
  `cena` int(10) NOT NULL,
  `skladem` int(10) NOT NULL,
  `doplatek` int(11) DEFAULT '0',
  PRIMARY KEY (`kod`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Vypisuji data pro tabulku `lek`
--

INSERT INTO `lek` (`kod`, `nazev`, `na_predpis`, `cena`, `skladem`, `doplatek`) VALUES
(1066, 'Framykoin', 1, 150, 64, 20),
(1093, 'Penicilin', 1, 80, 88, 20),
(3128, 'Stopangin', 0, 100, 0, 20),
(3378, 'Biseptol', 1, 125, 94, 20),
(9414, 'Nasivin', 0, 101, 30, 20),
(32081, 'Ibalgin', 0, 105, 20, 20),
(47711, 'Coldrex', 0, 119, 10, 20);

-- --------------------------------------------------------

--
-- Struktura tabulky `obsahoval`
--

CREATE TABLE IF NOT EXISTS `obsahoval` (
  `lek_kod` int(11) NOT NULL,
  `prodej_id` int(11) NOT NULL,
  `predepsany` tinyint(4) NOT NULL,
  KEY `lek_kod` (`lek_kod`),
  KEY `prodej_id` (`prodej_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Struktura tabulky `pojistovna`
--

CREATE TABLE IF NOT EXISTS `pojistovna` (
  `cislo` int(10) NOT NULL,
  `nazev` varchar(100) NOT NULL,
  `cislo_uctu` int(10) NOT NULL,
  `kod_banky` int(10) NOT NULL,
  `email` varchar(100) NOT NULL,
  PRIMARY KEY (`cislo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Vypisuji data pro tabulku `pojistovna`
--

INSERT INTO `pojistovna` (`cislo`, `nazev`, `cislo_uctu`, `kod_banky`, `email`) VALUES
(0, 'Bez předpisu', 0, 0, 'vodl@centrum.cz'),
(111, 'VZP', 2147483647, 100, 'vodl@centrum.cz'),
(201, 'Vojenská', 2147483647, 100, 'vodl@centrum.cz'),
(205, 'Hutnická', 2147483647, 100, 'vodl@centrum.cz'),
(211, 'Vnitra', 2147483647, 100, 'vodl@centrum.cz');

-- --------------------------------------------------------

--
-- Struktura tabulky `prodej`
--

CREATE TABLE IF NOT EXISTS `prodej` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `datum` datetime NOT NULL,
  `exportovano` datetime DEFAULT NULL,
  `pojistovna_cislo` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `pojistovna_cislo` (`pojistovna_cislo`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=75 ;

-- --------------------------------------------------------

--
-- Struktura tabulky `user`
--

CREATE TABLE IF NOT EXISTS `user` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `password` char(60) NOT NULL,
  `name` varchar(100) NOT NULL,
  `role` varchar(10) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5 ;

--
-- Vypisuji data pro tabulku `user`
--

INSERT INTO `user` (`id`, `username`, `password`, `name`, `role`) VALUES
(2, 'admin', '$2a$07$c35m4wopyfm5ykjwuih05uSzFVAMcXt1xotcwFTtATXkbn6T4FCHy', 'Administrator', 'admin'),
(3, 'user', '$2a$07$8nnro7y3fmov4gnee9585ur1IjzYVHqa2gbisoBubL.HcacRa5lF2', 'User', 'user'),
(4, 'lekarnik', '$2a$07$64zuaqyo95fwbxecd957bunc3emPWudAuenehRnAy15veBnV4bHye', 'Lekarnik Tonda', 'user');

--
-- Omezení pro exportované tabulky
--

--
-- Omezení pro tabulku `doplaci`
--
ALTER TABLE `doplaci`
  ADD CONSTRAINT `doplaci_ibfk_1` FOREIGN KEY (`lek_kod`) REFERENCES `lek` (`kod`) ON DELETE CASCADE ON UPDATE NO ACTION,
  ADD CONSTRAINT `doplaci_ibfk_2` FOREIGN KEY (`pojistovna_cislo`) REFERENCES `pojistovna` (`cislo`) ON DELETE CASCADE ON UPDATE NO ACTION;

--
-- Omezení pro tabulku `obsahoval`
--
ALTER TABLE `obsahoval`
  ADD CONSTRAINT `obsahoval_ibfk_1` FOREIGN KEY (`lek_kod`) REFERENCES `lek` (`kod`) ON DELETE CASCADE ON UPDATE NO ACTION,
  ADD CONSTRAINT `obsahoval_ibfk_2` FOREIGN KEY (`prodej_id`) REFERENCES `prodej` (`id`) ON DELETE CASCADE ON UPDATE NO ACTION;

--
-- Omezení pro tabulku `prodej`
--
ALTER TABLE `prodej`
  ADD CONSTRAINT `prodej_ibfk_1` FOREIGN KEY (`pojistovna_cislo`) REFERENCES `pojistovna` (`cislo`) ON DELETE CASCADE ON UPDATE NO ACTION;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
