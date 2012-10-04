SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;




CREATE TABLE IF NOT EXISTS timetracker_projects (
  id int(10) NOT NULL auto_increment,
  project_group_id int(10) NOT NULL default '0',
  project_id int(10) default NULL,
  project_name varchar(255) default NULL,
  project_external_key varchar(32) default NULL,
  project_desc varchar(255) default NULL,
  project_hours int(4) default NULL,
  created datetime NOT NULL,
  `changed` datetime NOT NULL,
  vid int(10) NOT NULL,
  `status` int(4) NOT NULL default '0',
  PRIMARY KEY  (id),
  UNIQUE KEY project_external_key (project_external_key)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=149 ;



CREATE TABLE IF NOT EXISTS timetracker_project_events (
  id int(10) NOT NULL auto_increment,
  user_id int(10) default NULL,
  project_id int(10) default NULL,
  project_name varchar(32) default NULL,
  project_user_task varchar(255) default NULL,
  event_date date default NULL,
  project_hours int(4) default NULL,
  created datetime NOT NULL,
  `changed` datetime NOT NULL,
  PRIMARY KEY  (id)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=89 ;



CREATE TABLE IF NOT EXISTS timetracker_project_groups (
  project_group_id int(10) NOT NULL auto_increment,
  project_group_name varchar(32) NOT NULL,
  project_group_prefix varchar(12) NOT NULL,
  PRIMARY KEY  (project_group_id)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=7 ;



CREATE TABLE IF NOT EXISTS timetracker_project_status (
  id int(10) NOT NULL auto_increment,
  status_name varchar(16) NOT NULL,
  PRIMARY KEY  (id)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=4 ;
