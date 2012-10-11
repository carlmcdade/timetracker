<?php

/*
 * @author Carl McDade
 * @copyright Copyright (c) 2010, Carl McDade
 * @since 2010-02-19
 * @version 0.2
 * @license http://opensource.org/licenses/gpl-license.php GNU Public License
 * @link http://www.drupal.se Drupal Sweden
 *
 * This is a beta application and many functions are created to circumvent Drupal's
 * default way of doing things (forms api) because they to not allow for the functionality needed 
 *
 */


class timetracker{
	
	protected $common;

    function __construct()
    {
        /* Test the Common settings */
        $this->common = Common::get_instance();
    }

    function hook_links()
    {
        
        $section = get_class($this);
    	$var[ $section .'_1'] = array(
        		'text' => 'My time', 
        		'path' => 'timetracker/timetracker/user'
        		);
        
        $var[$section .'_2'] = array(
        		'text' => 'Reports', 
        		'path' => 'timetracker/timetracker/reports'
        		);
        
        $var[$section .'_3'] = array(
        		'text' => 'Projects', 
        		'path' => 'timetracker/timetracker/projects'
        		);

        return $var;
    }
    
    
    
    function user()
    {        
    	
    	/*
    		js files
    		drupal_add_library('system', 'ui.slider');
    		drupal_add_library('system','ui.datepicker');
    		the jquery add-ons needed for the UI.
    	*/ 
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/jquery.form.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/jquery.validate.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/additional-methods.js');
        drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/anytime.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/mvc.js');
    	
    	
    	/*
    		style sheets
    	*/
    	drupal_add_css(drupal_get_path('module', APP_MODULE ) . '/css/anytime.c.css');
    	drupal_add_css(drupal_get_path('module', APP_MODULE ) . '/css/default.css');
    	
        if(variable_get('timetracker_projects', '') == '')
		{
			/* menu on the timetracker page from hook_links() function */
			$variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
			$variables['content'] = t('There are no projects at this time. Contact the Administator.');
		}
		else
		{
			/* this is linked to the menu system so no title is necessary */
			$variables['title'] = t( "Things you've done so far");
			
			
			/* menu on the timetracker page from hook_links() function */
			$variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
			
			
			/* 
				the entry form is in the module file because Drupal uses a string to find the function
				and cannot be called from within a class
			*/
			
			$output = drupal_render(drupal_get_form('timetracker_mytime_'));
			
			/* list the user projects table */
			$output .=  '<div id="output"></div>';
			
			$output .= self::dailyreport();
									
			$variables['content'] = $output;
		}
    	
    	return timetracker_view('default', $variables);	
        
    }
    
    function dailyreport($today = FALSE, $date = NULL)
    {
    	global $user;
    	
    	$total_hours = 0;
    	/* create select list of projects */
		// get vocabulary as options list
		$voc = taxonomy_vocabulary_machine_name_load(variable_get('timetracker_projects', 'projects'));
		$tree = taxonomy_get_tree($voc->vid,0,NULL,true);
			
    	
    	
    	if($today)
    	{
    		$table_header = array(
				array('data' => 'ID'),
				array('data' => 'Task'),
				array('data' => 'Project'),
				array('data' => 'Date'),
				array('data' => 'Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
    		
    		/* limit to five days */
    		$result = db_query("SELECT *
			FROM {timetracker_project_events} e WHERE e.user_id = :uid AND event_date = '" . $date. "' ORDER BY created DESC LIMIT 1", array(':uid' => $user->uid));
			
			$all = $result->fetchAll();
			
			foreach ($all as $key => $record)
			{
					  
				  $output = '<tr class="event-added" id="recent-' . $record->id . '">' .
					  		'<td class="cell-1-' . $record->id . '">' . $record->id . '</td>' . 
							'<td class="cell-2-' . $record->id . '">' . $record->project_user_task . '</td>' . 
							'<td class="cell-3-' . $record->id . '">' . $record->project_name . '</td>' . 
							'<td class="event-date cell-4-' . $record->id . '"><a href="#'. $record->event_date .'">' . $record->event_date . '</a></td>' .
							'<td class="cell-5-' . $record->id . '">' . $record->project_hours . '</td>' .
							'<td class="cell-6-' . $record->id . '">' .
									l(t('delete'), 'timetracker/timetracker/delete_time/' . $record->id . '/' . $record->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $record->id))) . '</td>' .
							'<td class="cell-7-' . $record->id . '">' .
									l(t('update'), '', array('attributes' => array('class' => array('update-recent-time'), 'id' => 'upd-' . $record->id))) . '</td>' .					  
							'</tr>'. "\n" .
							'<tr id="recent-form-row-'.$record->id .'" style="display:none" class=" temp-form">' . 
							'<td colspan="7"><div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
								'<form method="post" id="re-update-entry-'.$record->id.'" action="http://drupal.se/timetracker/timetracker/update_mytime">'.
								'<table><tr>' .
								'<td><label>'.t('Task').'<input type="textfield" id="re-task-'.$record->id.'" name="task" value="'.$record->project_user_task.'" size="24"></label></td>' .
								'<td><label>' . t('Projects') . '' . '</label></td>' .
								'<td>' .
								'<input type="hidden" name="form-id" id="re-form-id-'.$record->id.'" value="'.$record->id.'"></td>' .
								'<td><label>'.t('Hours').'<input type="textfield" id="re-hours-'.$record->id.'" name="hours" size="2" value="'.$record->project_hours.'"></label>' .
								'<td><input type="button" class="recentUpdate" value="update"></td>' .
								'<td class="indicator"><div class="working-indicator"></div></td>' .
								'</tr></table>'.
								'</form>' .
							 '</td></tr>';	  
			}
						 
		  	return $output;
			
    	}
    	else
    	{
    	
    		
    	$recent_header = array(
				array('data' => 'ID'),
				array('data' => 'Task'),
				array('data' => 'Project'),
				array('data' => 'Entry Date'),
				array('data' => 'Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
    		
		
		
		
		/* show the user 5 of the most recent events added */
    	$query = db_query('SELECT *
			FROM {timetracker_project_events} e WHERE e.user_id = :uid ORDER BY created DESC LIMIT 5', array(':uid' => $user->uid));
		
		$recent = $query->fetchAll();
		
		foreach ($recent as $re_record)
		{
				  
			$selected = '';  
			$terms = '<select id="re-projects-'.$re_record->id.'" name="projects">'; 
			$terms .= '<option value="0"></option>';
					foreach ($tree as $key => $term)
					{
						if($term->tid === $re_record->project_id)
						{
							$selected = 'selected="selected"';
						}
						$terms .= '<option value="'.$term->tid.'" '.$selected.'>' . $term->name . '</option>';
						$selected = '';
					}
			$terms .= '</select>'; 	  
			
			$event_data[] = array(
				'data' => array(
					array('data' => $re_record->id, 'class' => 'cell-1-'.$re_record->id), 
					array('data' => $re_record->project_user_task, 'class' => 'cell-2-'.$re_record->id), 
					array('data' => $re_record->project_name, 'class' => 'cell-3-'.$re_record->id), 
					array('data' => l($re_record->event_date, '',  array('fragment' => $re_record->event_date, 'external' => TRUE)), 'class' => 'cell-4-'.$re_record->id),
					array('data' => $re_record->project_hours, 'class' => 'cell-5-'.$re_record->id),
					array('data' => l(t('delete'), 'timetracker/timetracker/delete_mytime/' . $re_record->id . '/' . $re_record->event_date, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $re_record->id))), 'class' => 'cell-6-'.$re_record->id),
					array('data' => l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$re_record->id, array('attributes' => array('class' => array('update-recent-time'), 'id' => 'upd-' . $re_record->id))), 'class' => 'cell-7-'.$re_record->id) 
				),
				'id' => array('id' => 'recent-'.$re_record->id),
						  );
			$event_data[] = array(
							'data' => array(
								
								array('data' => '<div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
									'<form method="post" id="re-update-entry-'.$re_record->id.'" action="http://drupal.se/timetracker/timetracker/update_mytime">'.
									'<table><tr>' .
									'<td><label>'.t('Task').'<input type="textfield" id="re-task-'.$re_record->id.'" name="task" value="'.$re_record->project_user_task.'" size="12"></label></td>' .
									'<td><label>' . t('Projects') . '' . $terms . '</label></td>' .
									'<td>' .
									'<input type="hidden" name="form-id" id="re-form-id-'.$re_record->id.'" value="'.$re_record->id.'"></td>' .
									'<td><label>'.t('Hours').'<input type="textfield" id="re-hours-'.$re_record->id.'" name="hours" size="2" value="'.$re_record->project_hours.'"></label>' .
									'<td><input type="button" class="recentUpdate" value="update"></td>' .
									'<td class="indicator"><div class="working-indicator"></div></td>' .
									'</tr></table>'.
									'</form>', 
									'colspan' => 7)),
								'id' => array('id' => 'recent-form-row-'.$re_record->id),
								'style' => array('style' => 'display:none'),
						);
				  
				  
		}	
		
		$output[] = '<div id="most-recent-events"><h2>' . t('Most recent entries') . '</h2>' . theme('table', array(
			'header' => $recent_header,
			'rows' => $event_data,
			
		  )) . '</div>';
		
		
		$table_header = array(
				array('data' => 'ID'),
				array('data' => 'Task'),
				array('data' => 'Project'),
				array('data' => ''),
				array('data' => 'Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
		
		/*  limit to five days */
    	$result = db_query('SELECT *
			FROM {timetracker_project_events} e WHERE e.user_id = :uid AND event_date <= date (now ()) ORDER BY event_date DESC, id DESC', array(':uid' => $user->uid));
				
		
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all = $result->fetchAll();
		/* check to see if there are any entries for today */
		
		
		
		
		
		foreach ($all as $key => $record)
		{
			$groups[$record->event_date][] = $record;
		}
			
		
		foreach($groups as $key => $group)
		{
			//sections by date
			$output[] = '<div id="table-' . $key . '">' . 
		
			'<div class="table-label"><a name="' . $key . '" href=""><span class="date">' . $key . '</span></a><span class="hours"></span></div>';
			
			$date = $key;
		
		
		foreach ($group as $record)
		{
				$selected = '';  
				$terms = '<select id="projects-'.$record->id.'" name="projects">'; 
				$terms .= '<option value="0"></option>';
					foreach ($tree as $key => $term)
					{
						if($term->tid === $record->project_id)
						{
							$selected = 'selected="selected"';
						}
						$terms .= '<option value="'.$term->tid.'" '.$selected.'>' . $term->name . '</option>';
						$selected = '';
					}
				$terms .= '</select>';  
			
				$row_data[] = array(
						  'data' => array(
						  	  array('data' => $record->id, 'class' => 'cell-1-'.$record->id), 
						  	  array('data' => $record->project_user_task, 'class' => 'cell-2-'.$record->id), 
						  	  array('data' => $record->project_name, 'class' => 'cell-3-'.$record->id),
						  	  array('data' => '<input type="hidden" name="my_projects_id" id="my-projects-id" value="' .$record->project_id . '">'),
						  	  array('data' => $record->project_hours, 'class' => 'cell-5-'.$record->id),
						  	  array('data' => l(t('delete'), 'timetracker/timetracker/delete_mytime/' . $record->id . '/' . $record->event_date, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $record->id))), 'class' => 'cell-6-'.$record->id),
						  	  array('data' => l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$record->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $record->id))), 'class' => 'cell-7-'.$record->id) 
				  
						  	  ),
						  'class' => array('class' => 'row-'.$record->id),
						  );
				  $row_data[] = array(
							'data' => array(
								
								array('data' => '<div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
									'<form method="post" id="update-entry-'.$record->id.'" action="http://drupal.se/timetracker/timetracker/update_time">'.
									'<table><tr>' .
									'<td><label>'.t('Task').'<input type="textfield" id="edit-task-'.$record->id.'" name="task" value="'.$record->project_user_task.'" size="24"></label></td>' .
									'<td><label>' . t('Projects') . '' . $terms . '</label></td>' .
									'<td>' .
									'<input type="hidden" name="form-id" id="form-id-'.$record->id.'" value="'.$record->id.'"></td>' .
									'<td><label>'.t('Hours').'<input type="textfield" id="edit-hours-'.$record->id.'" name="hours" size="2" value="'.$record->project_hours.'"></label>' .
									'<td><input type="button" id="sendUpdate" value="update"></td>' .
									'<td class="indicator"><div class="working-indicator"></div></td>' .
									'</tr></table>'.
									'</form>', 
									'colspan' => 7)),
								'id' => array('id' => 'form-row-'.$record->id),
								'style' => array('style' => 'display:none'),
						);
				  
				  $total_hours += $record->project_hours;
				  
		}
		
		$row_data[] = array(
							'data' => array(
								'',
								'',
								'',
								'Total Hours:', 
								array('data' => $total_hours , 'colspan' => 3, 'id' => 'total-user-hours')),
							'id' => array('id' => 'total-row-'.$date),
						);
		
		
		
		
		
		
		/* Format and print out the table. */
		
		
		$output[] = theme('table', array(
			'header' => $table_header,
			'rows' => $row_data,
			'id' => 'table-articles',
			'class' => 'articles',
		  )) . '</div>';
		
		unset($row_data);
		$total_hours = 0;
		
		}
		}
    					 
    	return implode('', $output);
    	
    }
    
    /* this form posts to myreport() via ajax */

    function reports()
    {  	
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/jquery.form.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/anytime.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/mvc.js');   	
    	
    	/* title */
        $variables['title'] = 'My time reports';
    	
    	/* links */
        $variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
        
        /* form for choosing a project report */
        $output = drupal_render(drupal_get_form('timetracker_myreports_'));
        
    	/* export csv */
		$output .= '<div id="export-form" style="display:none">' . drupal_render(drupal_get_form('timetracker_myreports_csv_')) . '</div>';
			
    	/* monthly totals for this user by project */
        $variables['content'] = $output;

        return timetracker_view('default', $variables);
        
    }
    
    /* this is the response to the ajax call for the reports() function/form */
    
    function myreport($project = NULL)
    {
    	global $user;
    	
    	$total_hours = 0;
    	
    	/* clean input */
    	if(isset($_POST['my_projects_id']) && $_POST['my_projects_id'] != '')
    	{
    		$project = check_plain($_POST['my_projects_id']);   		
    		
			/*  get the term name */
			$project_info = taxonomy_term_load($project);
			
			$result = db_query('SELECT *
				FROM {timetracker_project_events} e WHERE e.user_id = :uid AND e.project_id = :pid ORDER BY created DESC', array(':uid' => $user->uid,':pid' => $project));
				
    	}
    	else
    	{
    		//
    		$result = db_query('SELECT *
				FROM {timetracker_project_events} e WHERE e.user_id = :uid AND event_date = date(now()) ORDER BY created DESC', array(':uid' => $user->uid));
			
    	}
    		
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all = $result->fetchAll();
		
		$table_header = array(
				array('data' => 'ID', '' => 'id', 'sort' => 'asc'),
				array('data' => 'Task', '' => 'project_user_task'),
				array('data' => 'Date', '' => 'event_date'),
				array('data' => 'Hours', '' => 'project_hours'),
				array('data' => 'Operations', '' => '', 'colspan' => 2),			
				);
		
		foreach ($all as $record)
		{
				  
				  $row_data[] = array(
						  'data' => array(
						  	  array('data' => $record->id, 'class' => 'cell-1-'.$record->id), 
						  	  array('data' => $record->project_user_task, 'class' => 'cell-2-'.$record->id), 
						  	  array('data' => $record->event_date, 'class' => 'cell-3-'.$record->id),
						  	  array('data' => $record->project_hours, 'class' => 'cell-4-'.$record->id),
						  	  array('data' => l(t('delete'), 'timetracker/timetracker/delete_time/' . $record->id . '/' . $project, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $record->id))), 'class' => 'cell-5-'.$record->id),
						  	  array('data' => l(t('update'), APP_MODULE .'/timetracker/update_time/'.$record->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $record->id))), 'class' => 'cell-6-'.$record->id) 
				  
						  	  ),
						  'id' => array('id' => 'row-'.$record->id),
						  );
				  $total_hours += $record->project_hours;
		}
		
		$row_data[] = array(
							'data' => array(
								'',
								'',
								'Total Hours:', 
								array('data' => $total_hours , 'colspan' => 3, 'id' => 'total-user-hours')),
						);
		
		$row_data[] = array(
							'data' => array(
								
								array('data' => '<div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
									'<form method="post" style="display:none" id="update-mini-form" action="' . 'http://drupal.se/timetracker/timetracker/update_time">'.
									'<table><tr>' .
									'<td><label>'.t('Task').'<input type="textfield" id="edit-task" name="title" value="" size="12"></label></td>' .
									'<td><label>'.t('Date').'<input type="textfield" id="edit-event-date" name="event_date" value="" size=""></label>' .
									'<input type="hidden" name="id_mini_form" id="id-mini-form" value=""></td>' .
									'<td><label>'.t('Hours').'<input type="textfield" id="edit-hours" name="hours" size="2" value=""></label>' .
									'<td><input type="button" id="sendUpdate" value="update"></td>' .
									'</tr></table>'.
									'</form>', 
									'colspan' => 6)),
								'id' => array('id' => 'mini-form-row'),
								'style' => array('style' => 'display:none'),
						);
		
		
		/* Format and print out the table. */
		$output .= theme('table', array(
			'header' => $table_header,
			'rows' => $row_data,
			'id' => 'table-articles',
			'class' => 'articles',
		  ));
		
    					 
    	print $output;
    	
    }
    
    /* the response to the call for the export form */
    
    function myreport_csv($result,  $filename = NULL)
    { 
    	global $user;
    	
    	
    	/*
    		Drupal 6
    		drupal_set_header('Content-Type: text/csv');
    		drupal_set_header('Content-Disposition: attachment; filename=' . $filename);
    	*/
    	
    	
    	
    	global $user;
    	 	
    	$total_hours = 0;
    	
    	/* clean input */
    	$project = check_plain($_POST['my_report_id']);
    	
    	/*  get the term name */
    	$project_info = taxonomy_term_load($project);
    	
    	/* build the file name */
    	$filename = strtolower($user->name . '-project-' . $project_info->tid . '-' . date("Y-m-d") . '.txt');
    	
    	/* cleanup file name from commonly used special characters */
    	$not_used = array(" ", "_", "&","[","]");
    	$filename = str_replace($not_used, "-", $filename);
    	
    	/*  set content headers */
    	drupal_add_http_header('Content-Type', 'text/csv; utf-8', TRUE);
    	drupal_add_http_header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    	
    	$result = db_query('SELECT *
			FROM {timetracker_project_events} e WHERE e.user_id = :uid AND e.project_id = :pid ORDER BY created DESC', array(':uid' => $user->uid,':pid' => $project));
				
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all = $result->fetchAll();
    	
		/* field headers */
    	$count = 0; 
    	foreach($all as $x => $head){
    		foreach($head as $key => $value){
			  $header[$key] = $key; 
			}
			
			if($x > 0)
			{
				/* iterate only one record to get the database field names */
				break;
			}
    	}
    	
    	print implode(',', $header) ."\r\n";
    	
    	/* field values */
    	foreach($all as $record){
			foreach($record as $value){
			  $values[] = '"' . str_replace('"', '""', decode_entities(strip_tags($value))) . '"'; 
			}
			print implode(',', $values) ."\r\n";
			unset($values);
		}
		
	}
    
	/* this is a list to inform the user about the projects available */
    
    function projects()
    {
        /* title */
        $variables['title'] = 'My projects';  	
    	
    	/* links */
    	$variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
    	
    	$output = '<div id="project-list">' . t('A list of available projects and their descriptions:') . '</div>';
    	
    	/* get vocabulary as options list */
		$voc = taxonomy_vocabulary_machine_name_load(variable_get('timetracker_projects', 'projects'));
		$tree = taxonomy_get_tree($voc->vid,0,NULL,true);
				
		foreach ($tree as $key => $term)
		{
			$output .= '<dl><dt>' . $term->name . '</dt><dd>' . $term->description  . '</dd></dl>';												
		}
    	
    	/* show a list of the taxonomy terms used in the the vocbulary */
        $variables['content'] = $output;
        
        return timetracker_view('default', $variables);
    }
    
    
    
    /*  the response to the mini-form for updating an entry */
    
    function update_time()
    {
    	
    	global $user;    	
    	
    	/* id title hours date */
    	$pid = check_plain($_POST['pid']);
    	$id = check_plain($_POST['id']);
    	$hours = check_plain($_POST['hours']);
    	$task = check_plain($_POST['task']);
    	
    	/*  get the term name */
    	$project = check_plain($_POST['project']);
		$project_info = taxonomy_term_load($project);
    	
    	$event_update = db_update('timetracker_project_events') // Table name no longer needs {}
		  ->fields(array(
		    'project_user_task' => $task,
		    'project_name' => $project_info->name,
		    'project_id' => $project_info->tid,
			'project_hours' => $hours,
			'changed' => date("Y-m-d H:i:s")
		  ))
		  ->condition('id', $id, '=')
		  ->condition('user_id', $user->uid, '=')
		  ->execute();
		  
		if($event_update > 0)
		{
		 
		  /* limit to five days */
    	$result = db_query("SELECT *
			FROM {timetracker_project_events} e WHERE e.id = :id LIMIT 1", array(':id' => $id));
			
		$all = $result->fetchObject();
			
		
			$output = '<tr class="event-added" id="row-' . $all->id . '">' .
					  '<td class="cell-1-' . $all->id . '">' . $all->id . '</td>' . 
					  '<td class="cell-2-' . $all->id . '">' . $all->project_user_task . '</td>' . 
					  '<td class="cell-3-' . $all->id . '">' . $all->project_name . '</td>' . 
					  '<td class="event-date cell-4-' . $all->id . '">' . $all->event_date . '</td>' .
					  '<td class="cell-5-' . $all->id . '">' . $all->project_hours . '</td>' .
					  '<td class="cell-6-' . $all->id . '">' .
					  	l(t('delete'), 'timetracker/timetracker/delete_time/' . $all->id . '/' . $all->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $all->id))) . '</td>' .
					  '<td class="cell-7-' . $all->id . '">' .
						l(t('update'), APP_MODULE .'/timetracker/update_time/'.$all->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $all->id))) . '</td>' .
					  '</tr>';
			$row_id = $all->id;
			
		/* update the hours total for the day */
		$result_hours = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = :uid AND e.event_date = :eid', array(':uid' => $user->uid,':eid' => $all->event_date));
				
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all_hours = $result_hours->fetchAll();
		
		
		foreach ($all_hours as $record)
		{
			$total_hours += $record->project_hours;
		}	
		
    	
    	/* ajax response */
    	print $all->id . '}{' . $output . '}{' . $total_hours . '}{' . $all->event_date;
    	  
    	}
    	else
    	{
    		print t('error row not updated');
    	}
    }
    
    /* the response to the delete link on the list of entries */
    
    function delete_time()
    {
    	global $user;
    	
    	/* remove record from user database */
    	$id = check_plain(arg(3));
    	
    	$deleted = db_delete('timetracker_project_events')
		  ->condition('id', $id)
		  ->execute();
		  
		$total_hours = 0;
    	
    	/* clean input */
    	$project = check_plain(arg(4));
    	
    	/*  get the term name */
    	$project_info = taxonomy_term_load($project);
    	
    	$result = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = :uid AND e.project_id = :pid ORDER BY created DESC', array(':uid' => $user->uid,':pid' => $project));
				
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all = $result->fetchAll();
		
		
		foreach ($all as $record)
		{
			$total_hours += $record->project_hours;
		}
    	
    	print $total_hours;
    }
    
    /* the response to creating a time entry for a project */
    
    function mytime()
    {
    	
    	/*
    		the form is using the term id so getting the term object to put the term
    		info into the database
    	*/
    	$project_info = taxonomy_term_load($_POST['project_id']);
    	
    	/* clean the user input */
    	$user_task = check_plain($_POST['user_task']);
    	
    	/* clean user input */
    	$project_hours = check_plain($_POST['hours_used']);
    	
    	/* clean user input */
    	$event_date = check_plain($_POST['event_date']);
    	   	
    	
    	/* add record to user database */
    	global $user;
    	   	
    	$event = db_insert('timetracker_project_events') 
				->fields(array(
				  'user_id' => $user->uid,
				  'project_id' => $project_info->tid,
				  'project_name' => $project_info->name,
				  'project_user_task' => $user_task,
				  'event_date' => $event_date,
				  'project_hours' => $project_hours,
				  'created' => date("Y-m-d H:i:s"),
				  'changed' => date("Y-m-d H:i:s")
				))
				->execute();
				
		/* inform the user */
		//drupal_set_message(t( 'Confirming ') . $project_hours . t(' task hours added to ') . $project_info->name . t(' for ') . $event_date );
		
		/* show the updated report */
		print $event_date.'}{'. self::dailyreport(TRUE, $event_date);
    	  	
    }
    
    /* the response to the delete link on the list of entries */
    
    function delete_mytime()
    {
    	global $user;
    	
    	/* remove record from user database */
    	$id = check_plain(arg(3));
    	
    	$deleted = db_delete('timetracker_project_events')
		  ->condition('id', $id)
		  ->execute();
		  
		$total_hours = 0;
    	
    	/* clean input */
    	$date = check_plain(arg(4));
    	
    	$result = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = :uid AND e.event_date = :eid', array(':uid' => $user->uid,':eid' => $date));
				
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all = $result->fetchAll();
		
		
		foreach ($all as $record)
		{
			$total_hours += $record->project_hours;
		}
    	
    	print $date . '}{' . $total_hours;
    }
    
    /*  the response to the mini-form for updating an entry */
    
    function update_mytime()
    {
    	
    	global $user; 
    	
    	/* create select list of projects */
		// get vocabulary as options list
		$voc = taxonomy_vocabulary_machine_name_load(variable_get('timetracker_projects', 'projects'));
		$tree = taxonomy_get_tree($voc->vid,0,NULL,true);
		
    	
    	/* id title hours date */
    	$pid = check_plain($_POST['pid']);
    	$id = check_plain($_POST['id']);
    	$hours = check_plain($_POST['hours']);
    	$task = check_plain($_POST['task']);
    	
    	/*  get the term name */
    	$project = check_plain($_POST['project']);
		$project_info = taxonomy_term_load($project);
    	
    	$event_update = db_update('timetracker_project_events') // Table name no longer needs {}
		  ->fields(array(
		    'project_user_task' => $task,
		    'project_name' => $project_info->name,
		    'project_id' => $project_info->tid,
			'project_hours' => $hours,
			'changed' => date("Y-m-d H:i:s")
		  ))
		  ->condition('id', $id, '=')
		  ->condition('user_id', $user->uid, '=')
		  ->execute();
		  
		if($event_update > 0)
		{
		 
		  /* limit to five days */
    	$result = db_query("SELECT *
			FROM {timetracker_project_events} e WHERE e.id = :id LIMIT 1", array(':id' => $id));
			
		$all = $result->fetchObject();
			
		
			$daily = '<tr class="event-added row-' . $all->id . '">' .
					  '<td class="cell-1-' . $all->id . '">' . $all->id . '</td>' . 
					  '<td class="cell-2-' . $all->id . '">' . $all->project_user_task . '</td>' . 
					  '<td class="cell-3-' . $all->id . '">' . $all->project_name . '</td>' . 
					  '<td class="event-date cell-4-' . $all->id . '">' . $all->event_date . '</td>' .
					  '<td class="cell-5-' . $all->id . '">' . $all->project_hours . '</td>' .
					  '<td class="cell-6-' . $all->id . '">' .
					  	l(t('delete'), 'timetracker/timetracker/delete_time/' . $all->id . '/' . $all->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $all->id))) . '</td>' .
					  '<td class="cell-7-' . $all->id . '">' .
						l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$all->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $all->id))) . '</td>' .
					  '</tr>';
					  
			$recent = '<tr class="event-added recent-'. $all->id .'" id="recent-' . $all->id . '">' .
					  '<td class="cell-1-' . $all->id . '">' . $all->id . '</td>' . 
					  '<td class="cell-2-' . $all->id . '">' . $all->project_user_task . '</td>' . 
					  '<td class="cell-3-' . $all->id . '">' . $all->project_name . '</td>' . 
					  '<td class="event-date cell-4-' . $all->id . '"><a href="#' . $all->event_date . '">' . $all->event_date . '</a></td>' .
					  '<td class="cell-5-' . $all->id . '">' . $all->project_hours . '</td>' .
					  '<td class="cell-6-' . $all->id . '">' .
					  	l(t('delete'), 'timetracker/timetracker/delete_time/' . $all->id . '/' . $all->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $all->id))) . '</td>' .
					  '<td class="cell-7-' . $all->id . '">' .
						l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$all->id, array('attributes' => array('class' => array('update-recent-time'), 'id' => 'upd-' . $all->id))) . '</td>' .
					  '</tr>';
					  
			$row_id = $all->id;
			
		/* update the hours total for the day */
		$result_hours = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = :uid AND e.event_date = :eid', array(':uid' => $user->uid,':eid' => $all->event_date));
				
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all_hours = $result_hours->fetchAll();
		
		
		foreach ($all_hours as $record)
		{
			$total_hours += $record->project_hours;
		}	
		
    	
    	/* ajax response */
    	print $all->id . '}{' . $daily . '}{' . $total_hours . '}{' . $all->event_date . '}{' . $recent;
    	  
    	}
    	else
    	{
    		print t('error row not updated');
    	}
    }
    
    
    
}

?>
