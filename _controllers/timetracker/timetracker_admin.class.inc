<?php

/*
 * @author Carl McDade
 * @copyright Copyright (c) 2010, Carl McDade
 * @since 2010-02-19
 * @version 0.2
 * @license
 * @link 
 *
 */


class timetracker_admin{
	
	private $common;
	public  $page_title = 'Time Tracker Administration';
	public  $dbc;
	


    function __construct()
    {
        //Test the Common settings
        $this->common = Common::get_instance();
        $this->section = get_class($this);
    }

    function admin_links()
    {
        $section = get_class($this);
    	$var[ $section .'_1'] = array(
        		'text' => 'Entries', 
        		'path' => 'admin/timetracker/reports'
        		);
        
        $var[$section .'_2'] = array(
        		'text' => 'Projects', 
        		'path' => 'admin/timetracker/projects'
        		);
        

        return $var;
    }
    
    function index()
    {
    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
    	//
    	$variables['content'] = 'administration index';
    	return timetracker_view('default', $variables);  
    }
    
    
    function settings()
    {   	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
    	
        // this form is located in the timetracker module file
    	$output = drupal_render(drupal_get_form('timetracker_settings_form'));
    	$variables['content'] = $output;
        return timetracker_view('default', $variables);  	  	
    
    }
	
	function settings_save()
	{
		//
		variable_set('timetracker_projects', $_POST['selected']);
		drupal_set_message(t('The timetracker category is has been set to "'). variable_get('timetracker_projects', 'projects') . '".');
		drupal_goto('timetracker/admin/timetracker/settings');
	}
	
	function reports($extend = NULL)
    {
        // title for content
    	$variables['title'] = t('Administer Reports');
    	
        // admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
        
        $table_header = array(
				array('data' => 'ID', 'field' => 'id', 'sort' => 'asc'),
				array('data' => 'Task', 'field' => 'project_user_task'),
				array('data' => 'Name', 'field' => 'project_name'),
				array('data' => 'User', 'field' => 'user_id'),
				array('data' => 'Hours', 'field' => 'project_hours'),
				array('data' => 'Event', 'field' => 'event_date'),
				array('data' => 'Created', 'field' => 'created'),
				array('data' => 'Operations', '' => '', 'colspan' => 2),
				
				);
		//Create the Sql query. This uses various parts of Drupal 7's new DBTNG database abstraction layer.
			
		$query = db_select('timetracker_project_events', 'tt')
			->extend('PagerDefault') 	//Pager Extender
			->limit(10)				//10 results per page
			->extend('TableSort')		//Sorting Extender
			->orderByHeader($table_header)//Field to sort on is picked from $header
			->fields ('tt', 
				array(
				  	  'id',
				  	  'project_user_task', 
				  	  'project_name',
				  	  'user_id',
				  	  'project_hours',
				  	  'event_date',
				  	  'created'));
			
		$result = $query
			->execute();
		
		/*
		
		//Create a list of headers for your Html table (see Drupal 7 docs for theme_table here
			$header = array(
				array('data' => 'Title', 'field' => 'title', 'sort' => 'asc'),
				array('data' => 'Node ID', 'field' => 'nid'),
				array('data' => 'Type', 'field' => 'type'),
				array('data' => 'Created', 'field' => 'created'),
				array('data' => 'Published'),
				);
			
			//Create the Sql query. This uses various parts of Drupal 7's new DBTNG database abstraction layer.
			
			$query = db_select('node', 'n')
						->condition('status', 1)	//Only published nodes, change condition as it suits you
						->extend('PagerDefault') 	//Pager Extender
							->limit(10)				//10 results per page
						->extend('TableSort')		//Sorting Extender
							->orderByHeader($header)//Field to sort on is picked from $header
						->fields ('n', array (
							'nid',
							'title',
							'type',
							'created',
							'status',
						));
			
			$results = $query
						->execute();
			
			$rows = array();	
			foreach ($results as $node) {
				$rows[] = array(
							'data' => array(
								  l($node->title, 'node/'. $node->nid .'/edit'),
								  $node->nid,
								  $node->type,
								  format_date($node->created),
								  $node->status
							)
				);
			}
			
			//Theme the html table
			$html = theme('table', 
						array(
							'header' => $header,
							'rows'=>$rows,
							'caption' => 'Creating Drupal 7 style tables',	//Optional Caption for the table
							'sticky' => TRUE,						//Optional to indicate whether the table headers should be sticky
							'empty' => 'No nodes created...',		//Optional empty text for the table if resultset is empty
						)
					);
			
			//Append pager
			$html .= theme('pager',
						array(
							'tags' => array()
						)
					);
					
			return ($html);
					
					
		*/	
		
		// Result is returned as a iterable object that returns a stdClass object on each iteration
		
		foreach ($result as $record)
		{
				  
				  $row_data[] = array(
						  'data' => array(
						  	  $record->id,
						  	  $record->project_user_task, 
						  	  $record->project_name,
						  	  $record->user_id,
						  	  $record->project_hours,
						  	  $record->event_date,
						  	  $record->created,
						  	  l(t('delete'), '', array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $record->id))),
						  	  l(t('update'), APP_MODULE .'/timetracker/update_time/'.$record->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $record->id))) 
				  
						  	  ),
						  'id' => array('id' => 'row-'.$record->id),
						  );
		}
		
		// Format and print out the table.
		$output = theme('table', array(
			'header' => $table_header,
			'rows' => $row_data,
			'id' => 'table-articles',
			'class' => 'articles',
		  ));
		
		$output .= theme('pager',
						array(
							'tags' => array()
						)
					);
    	
    	// monthly totals for this user by project
        $variables['content'] = $output;

        return timetracker_view('default', $variables);
    }
    
    /**
    * @author Carl McDade
    * @name project administration
    * Programmatically add and delete taxonomy terms 
    * separately from the default taxonomy admin 
    * so that Managers do not get full backend previleges
    *
    */
    
    function projects($extend = NULL)
    {
        // title for content
    	$variables['title'] = t('');

    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
        
        // show projects add form 
        $output = drupal_render(drupal_get_form('timetracker_projects_add_'));
        
        $output .= '<div id="project-list">' . t('A list of available projects and their descriptions:') . '</div>';
    	
    	// get vocabulary as options list
		$voc = taxonomy_vocabulary_machine_name_load(variable_get('timetracker_projects', 'projects'));
		$tree = taxonomy_get_tree($voc->vid,0,NULL,true);
				
		
		/*  
		
		id 	int(10) 			No 	None 	auto_increment 	Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		project_group_id 	int(10) 			No 	None 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		project_id 	int(10) 			Yes 	NULL 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		project_name 	varchar(255) 	utf8_general_ci 		Yes 	NULL 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		project_external_key 	varchar(32) 	utf8_general_ci 		Yes 	NULL 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		project_desc 	varchar(255) 	utf8_general_ci 		Yes 	NULL 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		project_hours 	int(4) 			Yes 	NULL 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		created 	datetime 			No 	None 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		changed 	datetime 			No 	None 		Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
		vid
		
		*/
		
		$table_header = array(
				array('data' => 'ID', 'field' => 'tid', 'sort' => 'asc'),
				array('data' => 'Group', 'field' => 'project_group_id'),
				array('data' => 'Name', 'field' => 'project_name'),
				array('data' => 'Code', 'field' => 'project_external_key'),
				array('data' => 'Description', 'field' => 'description'),
				array('data' => 'Hours', 'field' => 'project_hours'),
				array('data' => 'Created', 'field' => 'created'),
				array('data' => 'Status', 'field' => 'status_name'),
				array('data' => 'Operations', '' => '', 'colspan' => 2),
				
				);
		//Create the Sql query. This uses various parts of Drupal 7's new DBTNG database abstraction layer.
			
		$query = db_select('taxonomy_term_data', 'td')
		/*
		$query->innerJoin('timetracker_projects', 'tt', 'td.tid = tt.project_id');
		$query->join('timetracker_project_groups', 'g', 'g.project_group_id = tt.project_group_id'); //JOIN node with users
		$query->join('timetracker_project_status', 's', 'tt.status = s.id'); //JOIN node with users
						
		$query->fields ('tt', 
				array(
				  	  'id',
				  	  'project_group_id', 
				  	  'project_id',
				  	  'project_name',
				  	  'project_external_key',
				  	  'project_desc',
				  	  'project_hours',
				  	  'created',
				  	  'status'))
			->fields ('g', array('project_group_name', 'project_group_prefix'))
			->fields ('s')*/
			->condition('td.vid', $voc->vid, '=')
			->extend('PagerDefault')
			->limit(10)
			->extend('TableSort')
			->orderByHeader($table_header)
			->fields ('td',array('tid', 'name', 'description'));
		$query->leftJoin('timetracker_projects', 'tt', 'td.tid = tt.project_id');
		$query->leftJoin('timetracker_project_groups', 'g', 'g.project_group_id = tt.project_group_id');
		$query->leftJoin('timetracker_project_status', 's', 'tt.status = s.id');
		$query->fields ('tt', array('project_external_key', 'created', 'project_hours'));
		$query->fields ('g', array('project_group_name', 'project_group_prefix'));
		$query->fields ('s');
			
		$result = $query
			->execute();
						
			
		foreach ($result as $record)
		{
			
			/* get the total number of hours for each project */
			$hours_total = 0;
			
			$query_hours = db_select('timetracker_project_events', 'e')
				->condition('project_id',$record->tid,'=')
				->fields('e', array('project_hours'));
				$hours = $query_hours
					->execute();
					
			foreach($hours as $event)
			{
				$hours_total += $event->project_hours;
			}
			
			//$output .= print_r($record,1);
			$row_data[] = array(
				'data' => array(
					$record->tid,
					$record->project_group_name, 
					$record->name,
					$record->project_group_prefix . '-' . $record->project_external_key,
					$record->description,
					$hours_total,
					$record->created,
					$record->status_name,
					l(t('edit'), 'admin/'. APP_MODULE .'/project_edit/'.$record->tid, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $record->tid))) 
					),
					'id' => array(
						'id' => 'row-'.$record->tid),
					);
			
		}
		
		//Format and print out the table.
		
		$output .= theme('table', array(
			'header' => $table_header,
			'rows' => $row_data,
			'id' => 'table-articles',
			'class' => 'articles',
		  ));
		
		$output .= theme('pager',
						array(
							'tags' => array()
						)
					);
               
    	$variables['content'] = $output;
       
        return timetracker_view('default', $variables);
    }
    
    function project_edit()
    {
    	// title for content
    	$variables['title'] = t('');

    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
        
    	$output = drupal_render(drupal_get_form('timetracker_projects_edit_'));
    	$variables['content'] = $output;
        
        return timetracker_view('default', $variables);
    }
    
    function project_save($tid=NULL)
    {
    	/* we may need to know who by and when a project was created */
    	global $user;
    	
    	$voc = taxonomy_vocabulary_machine_name_load(variable_get('timetracker_projects', 'projects'));			

    	$term = new stdClass();
		$term->name = check_plain($_POST['project_name']);
		$term->description = check_plain($_POST['project_desc']);
		$term->vid = $voc->vid;
		
		/* save to the Drupal taxonomy system which creates a term/project */
		taxonomy_term_save($term);		
		
		print t('project taxonomy created'); 
		
		/* get the term and information that was inserted into the database */
		$new = $term->tid;
    			
    	$term_info = taxonomy_term_load($new);
    	
    	$project_info = db_insert('timetracker_projects') // Table name no longer needs {}
				->fields(array(
				  'project_id' => $new,
				  'project_name' => $term_info->name,
				  'project_desc' => $term_info->description,
				  'project_external_key' => check_plain($_POST['project_external_key']),// since the terms do not have fields we add fk an external table and hold the extra information there
				  'created' => date("Y-m-d H:i:s"),
				  'changed' => date("Y-m-d H:i:s"),
				  'vid' => $voc->vid
				))
				->execute();   	
		
		print t('project added to timetracker'); 
    }
    
    function project_update()
    {
    	/* we may need to know who by and when a project was created */
    	global $user;
    	
    	$voc = taxonomy_vocabulary_machine_name_load(variable_get('timetracker_projects', 'projects'));			
    	
    	
    	// update the taxonomy table
    	$term = new stdClass();
    	$term->tid = check_plain($_POST['project_id']);
		$term->name = check_plain($_POST['project_name']);
		$term->description = check_plain($_POST['project_desc']);
		$term->vid = $voc->vid;

		
		/* save to the Drupal taxonomy system which creates a term/project */
		taxonomy_term_save($term);

		$project_id = db_select('timetracker_projects', 'tp')
						->fields('tp')
						->condition('project_id', $term->tid, '=')
						->execute()
						->fetchObject();
						
		
    	// update the timetracker project table
    	if(is_object($project_id))
    	{
			$project_info = db_update('timetracker_projects') // Table name no longer needs {}
					->fields(array(
						'project_name' => $term->name,
						'project_desc' => $term->description,
						'project_group_id' => check_plain($_POST['project_group']),
						'project_external_key' => check_plain($_POST['project_external_key']),// since the terms do not have fields we add fk an external table and hold the extra information there
						'changed' => date("Y-m-d H:i:s"),
						'status' => check_plain($_POST['project_status']),))
					->condition('project_id', $term->tid, '=')					
					->execute();
					
			drupal_set_message( t('project updated'));
					
		}
		else
		{		
			$project_info = db_insert('timetracker_projects') // Table name no longer needs {}
					->fields(array(
					  'project_id' => $term->tid,
					  'project_group_id' => check_plain($_POST['project_group']),
					  'project_name' => $term->name,
					  'project_desc' => $term->description,
					  'project_external_key' => check_plain($_POST['project_external_key']),// since the terms do not have fields we add fk an external table and hold the extra information there
					  'created' => date("Y-m-d H:i:s"),
					  'changed' => date("Y-m-d H:i:s"),
					  'status' => check_plain($_POST['project_status']),
					  'vid' => $voc->vid
					))
					->execute(); 
					
			drupal_set_message( t('project and taxonomy tables syncronised.'));
				
		}		
		
		 
		drupal_goto('admin/timetracker/project_edit/'. $term->tid);
		
    }
    
    
}

?>
