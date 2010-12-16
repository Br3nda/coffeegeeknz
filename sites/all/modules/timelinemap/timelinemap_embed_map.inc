<?php
// $Id: timelinemap_embed_map.inc,v 1.2 2010/07/15 07:08:22 vesnaradivojevic Exp $

/**
 * Submit handler for the embed administration form.
 * @param $form
 * @param $form_state
 */
function timelinemap_admin_embeds_controller($form_state){
  $checked = FALSE;
  if(!empty($form_state['values']['visualizations'])){
    foreach($form_state['values']['visualizations'] as $value){
      if($value){
        $checked = TRUE;
        break;
      }
    }
  }
  if ($form_state['values']['op'] == 'Delete selected' && $checked) 
    return _timelinemap_admin_embeds_delete_confirm($form_state);
    
  return _timelinemap_admin_embeds_form($form_state);    
}

/**
 * Confirmation form for deleting the embed file
 * and corresponding database records
 * @param $form_state
 */
function _timelinemap_admin_embeds_delete_confirm($form_state){
  $desc = 'This action CAN NOT be undone.';
  // Tell the submit handler to process the form
  $form['process'] = array('#type' => 'hidden', '#value' => 'true');
  // Make sure the form redirects in the end
  $form['destination'] = array('#type' => 'hidden', '#value' => 'admin/content/timelinemap/embeds');
  $to_delete = array();
  foreach($form_state['values']['visualizations'] as $key => $value){
    if($value){
      $to_delete[]=$value;
    }
  }
  $form['embeds_to_delete'] = array('#type' => 'hidden', '#value' => implode(',', $to_delete));
  return confirm_form($form,
                      t('Are you sure you want to delete the selected embed files?'),
                      'admin/content/timelinemap/embeds',
                      $desc,
                      'Delete',
                      'Return');   
}

/**
 * Theme function for the the embed files table
 * @param $form
 */
function theme_timelinemap_get_my_visualizations($form){
  $select_header = theme('table_select_header_cell');
  $header = array(
    $select_header,
    t('Title'),
    t('Embed code'),
    t('Date published'),
    t('Operations'),
  );
 /*  if(module_exists('vidi'))
    $header[] = t('Operations'); */
  
  $output = '';
  $output .= drupal_render($form['options']);
  $rows = array();
  $counter = 0;
  foreach (element_children($form['title']) as $key) {
    $row = array();
    $row[] = drupal_render($form['visualizations'][$key]);
    $row[] = drupal_render($form['title'][$key]);
    $row[] = drupal_render($form['code'][$key]);    
    $row[] = drupal_render($form['date'][$key]);
    /* if(module_exists('vidi')) */
      $row[] = drupal_render($form['operations'][$key]);
    $rows[] = $row;
    $counter++;
  }
  if ($counter == 0) {
    /* if(module_exists('vidi')) */
      $rows[] = array(array('data' => t('No posts available.'), 'colspan' => '5'));
   /*  else
      $rows[] = array(array('data' => t('No posts available.'), 'colspan' => '4')); */
  }

  $output .= theme('table', $header, $rows);
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  $output .= drupal_render($form);

  return $output;
}

/**
 * Form for the administration of user created embed files.
 * Everyone who can make an embed file can administer their files.
 * @param $form_state
 */
function _timelinemap_admin_embeds_form($form_state){
  $limit = 20; 
  //If the form fail validation, remember the #action attribute
  //so it doesn't redirect to the ahah callback
  if (!isset($form_state['storage']['old_action'])) {
    $form_state['storage']['old_action'] = isset($form['#action']) ? $form['#action'] : $_SERVER['REQUEST_URI'];
  }  

  $script = "function popup(mylink, windowname){if (! window.focus)return true; var href; if (typeof(mylink) == 'string') href=mylink; else href=mylink.href; window.open(href, windowname, 'width=1000,height=800,scrollbars=yes');return false;}";
  drupal_add_js($script, 'inline');
  if(user_access('administer embeds')){
    $form['filter'] = array(
      '#type' => 'fieldset',
      '#title' => t('Fiter embeds'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,      
    );
    $results = db_query('SELECT uid, name FROM {users}');
    $users = array();
    while ($result = db_fetch_object($results)){
      $users[$result->uid] = $result->uid != 0 ? $result->name : "anonymous";
    }
    $form['filter']['per_user'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#default_value' => $form_state['values']['per_user'],
      '#title' => t('Fiter embeds'),
      '#options' => $users  
    );
    
    $form['filter_button'] = array(
      '#type' => 'submit',
      '#value' => t('Filter'),
      '#ahah' => array(
        'path'    => 'timelinemap/embed_ahah',
        'wrapper' => 'timelinemap-embed-table-ahah',
        'method' => 'replace',
        'effect' => 'fade'
      ),           
    );
    if(isset($_GET['per_user']))
      $form_state['values']['per_user'] = $_GET['per_user'];
    $sql = "SELECT te.fid, te.title, te.link_vidi, te.link_view, te.visualisation_code, te.datasource, f.filename, f.filepath, f.timestamp FROM timelinemap_embeds te LEFT JOIN files f ON te.fid = f.fid";
    if(!empty($form_state['values']['per_user'])){
      $sql .= " WHERE";
      
      foreach ($form_state['values']['per_user'] as $value)
        $sql .= " f.uid = %d OR";
        
      $sql = substr($sql, 0, strlen($sql) - 2);
      $results = pager_query($sql, $limit, 0, NULL, $form_state['values']['per_user']);
    }
    else
      $results = pager_query($sql, $limit, 0, NULL);
  }
  else{  
    global $user;
    $sql = "SELECT te.fid, te.title, te.link_vidi, te.link_view, te.visualisation_code, te.datasource, f.filename, f.filepath, f.timestamp FROM timelinemap_embeds te LEFT JOIN files f ON te.fid = f.fid WHERE f.uid = %d";
    $results = pager_query($sql, $limit, 0, NULL, $user->uid);  
  }
  
  $destination = drupal_get_destination();
  $visualizations = array();
  while ($result = db_fetch_object($results)) {
    $visualizations[$result->fid] = '';
    $form['data']['title'][$result->fid] = array('#value' => l($result->title, $result->filepath, array('attributes' => array('onClick' => 'return popup(this, \'My visualization\')'))));
    $form['data']['date'][$result->fid] =  array('#value' => format_date($result->timestamp));
    $form['data']['code'][$result->fid] =  array('#value' => $result->visualisation_code, '#prefix' => '<div style="font-size:9px;">', '#suffix' => '</div>',);

    $vidilink = FALSE; $viewlink = FALSE; $op_str = '';
    if (module_exists('vidi') && $result->link_vidi){
      $op_str = l('edit embed', $result->link_vidi, array('query' => array('fid' => $result->fid, 'datasource' => $result->datasource)));
      $vidilink = TRUE;
    }
    if ($result->link_view){
      $exists = FALSE; 
      $view_link = explode('#', $result->link_view);
      $link_arr = explode('/', $view_link[0]);
      if ($view = views_get_view($link_arr[4])){
        $display_name_arr = explode('-', $view_link[1]);
        if (array_key_exists($display_name_arr[2], $view->display))
          $exists = TRUE;
      }
      
      if ($vidilink && $exists){
        $op_str .= ' | ';
      }
      if ($exists)
        $op_str .= l('edit view', $view_link[0], array('fragment' => $view_link[1]));
    }
    $form['data']['operations'][$result->fid] = array('#value' => $op_str);
	
  }
  $form['data']['visualizations'] = array('#type' => 'checkboxes', '#options' => $visualizations);
  $form['data']['pager'] = array('#value' => theme('pager', NULL, $limit, 0));
  $form['data']['#theme'] = 'timelinemap_get_my_visualizations';
  $form['data']['#prefix'] = '<div id="timelinemap-embed-table-ahah">';
  $form['data']['#suffix'] = '</div>';
  $form['timelinemap_delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete selected'),    
  );
  return $form;  
}

/**
 * Ahah callback for list of embeded files 
 * (filters the embeds by user)
 */
function timelinemap_embed_ahah (){
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];

  $form = form_get_cache($form_build_id, $form_state);

  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id); 

  $data = $form['data'];
  $_GET['q'] = 'admin/content/timelinemap/embeds';
  $data['pager'] = array(
    '#value' => theme('pager', NULL, 3)
  );
  unset($data['#prefix'], $data['#suffix']);
  $output = theme('status_messages') . drupal_render($data);

  drupal_json(array('status' => TRUE, 'data' => $output)); 
}

/**
 * Submit handler for the embed administration form.
 * @param $form
 * @param $form_state
 */
function timelinemap_admin_embeds_controller_submit($form, &$form_state){
  if(!$form_state['clicked_button']['#id'] != 'edit-filter-button'){
    if(isset($form_state['values']['embeds_to_delete'])){
      $for_deleting = explode(',', $form_state['values']['embeds_to_delete']);
      foreach($for_deleting as $value){
        $filepath = db_result(db_query("SELECT filepath FROM {files} WHERE fid=%d", $value));
        $sql = "DELETE FROM {timelinemap_embeds} WHERE fid = %d";
        if(!db_query($sql, $value)){
          drupal_set_message(t('An error occured while deleting the record with fid @id from the timelinemap_embeds table!', array('@id' => $value)), 'error');
          return TRUE;
        }
        $sql = "DELETE FROM {files} WHERE fid = %d";
        if(!db_query($sql, $value)){
          drupal_set_message(t('An error occured while deleting the record with fid @id  from the files table!', array('@id' => $value)), 'error');
          return TRUE;
        } 
        if(!file_delete($filepath)){
          drupal_set_message(t('The file @path was NOT deleted!', array('@path' => $filepath)), 'error');
          return TRUE;
        }        
      }
      drupal_set_message(t('The file @path was deleted successfully!', array('@path' => $filepath)), 'status');
      return TRUE;
    }
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * @param $uid
 * @param $filename. The name of the embed file.
 * @param $filepath. Relative path of the embed file.
 * @param $storage. The state of the form from the VIDI module. Used to populate the form with correct data.
 * @param $vidi_link. link to the VIDI module form.
 * @param $view_link. Link to the corresponding view and display.
 * @param $datasource. Timelinemap type (basic or pathline).
 * @param $result. Embed code.
 * @param $update. Is it an update or a creation of a new embed file.
 * @param $title. The title of the visualization. Displayed on the embed administration form.
 */
function _timelinemap_save_file_data ($uid, $filename, $filepath, $storage, $vidi_link, $view_link, $datasource, $result, $update, $title){
  $file = new stdClass();
  $file->uid = $uid;
  $file->filesize = filesize($filepath);
  $file->timestamp = time();
  
  if(!$update){
    $file->filepath = $filepath;
    $file->filename = $filename;
    $file->filemime = file_get_mimetype($filepath);
    $file->status = 1;
    drupal_write_record('files', $file);
    $timelinemap_embed_data = new stdClass();
    $timelinemap_embed_data->fid = $file->fid;
    $timelinemap_embed_data->datasource = $datasource;
    if(!empty($storage)){
      $timelinemap_embed_data->form_state = $storage;
      $un_storage = unserialize($storage);
      $timelinemap_embed_data->title = $un_storage['visualization_title'];
	} else{
	  $timelinemap_embed_data->title = $title;
	}
	if(!empty($vidi_link))
      $timelinemap_embed_data->link_vidi = $vidi_link;
    if(!empty($view_link))
      $timelinemap_embed_data->link_view = $view_link;
    $timelinemap_embed_data->visualisation_code = $result; 

    drupal_write_record('timelinemap_embeds', $timelinemap_embed_data);    
  }
  else{
    $sql = "SELECT fid FROM {files} WHERE filepath='%s'";
    $file->fid = db_result(db_query($sql, $filepath));
    drupal_write_record('files', $file, 'fid');
    $timelinemap_embed_data = new stdClass();
    if (!empty($storage))
      $timelinemap_embed_data->form_state = $storage; 
    if (!empty($vidi_link))
      $timelinemap_embed_data->link_vidi = $vidi_link;
    if (!empty($view_link))
      $timelinemap_embed_data->link_view = $view_link;  
    $un_storage = unserialize($storage);	
	$timelinemap_embed_data->title = $un_storage['visualization_title'];	
    $timelinemap_embed_data->fid = $file->fid;
    $timelinemap_embed_data->visualisation_code = $result;
    drupal_write_record('timelinemap_embeds', $timelinemap_embed_data, 'fid');
  }
}
/**
 * Ajax calback that makes the embed file,
 * inserts corresponding data in the database
 * and returns the embed code to the user.
 */
function timelinemap_embed(){
  global $user; global $base_url;  
  $id = $_POST['id']; 
  $rel = $_POST['rel']; 
  $div_id = $_POST['divId'];
  $settings = $_POST['settings']; 
  $timelinecontainer_style = $_POST['timelinecontainerStyle'];
  $mapcontainer = $_POST['mapcontainer']; 
  if($div_id){
	  $css = '<link type="text/css" rel="stylesheet" media="all" href="' . $base_url . '/' . drupal_get_path('module', 'timelinemap') . '/css/timelinemap.css"/>';  
	  $js = '<script type="text/javascript" src="' . $base_url . '/misc/jquery.js"></script>';
	  $js .= '<script type="text/javascript" src="' . $base_url . '/misc/drupal.js"></script>'; 
	  $gmap_apikey = variable_get('googlemap_api_key', '');
	  $timelinemap_apikey = $gmap_apikey ? $gmap_apikey : variable_get('timelinemap_api_key', ''); 
	  $timelinemap_gapi = 'http://maps.google.com/maps?file=api&v=2&key='. $timelinemap_apikey;
	  $timelinemap_sapi = 'http://static.simile.mit.edu/timeline/api/timeline-api.js';  
	  $js .= '<script type="text/javascript" src="'. $timelinemap_gapi .'"></script>';
	  $js .= '<script type="text/javascript" src="'. $timelinemap_sapi .'"></script>';
	  $js .= '<script type="text/javascript" src="' . $base_url . '/' . drupal_get_path('module', 'timelinemap') . '/js/timemap.js"></script>';
	  $js .= '<script src="'. check_url(url('http://js.mapbox.com/g/2/mapbox.js')) .'" type="text/javascript"></script>';
	  if($_POST['source'] == 'viditimelinemap')
		$js .= '<script type="text/javascript" src="' . $base_url . '/' . drupal_get_path('module', 'vidi') . '/js/vidi_timelinemap.js"></script>';
	  else
		$js .= '<script type="text/javascript" src="' . $base_url . '/' . drupal_get_path('module', 'timelinemap') . '/js/timelinemap.js"></script>';
	  
	  $js .= '<script type="text/javascript">' . $settings . '</script>';
	  
	  $output = '<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" />' . $css . $js . '<style>img, a img {border:none}</style></head>';
	  if($_POST['source'] == 'viditimelinemap')
		$output .= '<body><div id="timelinemap-all-' . $div_id . '" class = "viditimelinemap-all">';
	  else
		$output .= '<body><div id="timelinemap-all-' . $div_id . '" class = "timelinemap-all">';
	  $output .= '<div class = "timelinecontainer" style = "'.$timelinecontainer_style.'">' .
		  '<div id = "timeline-'. $div_id.'" class = "timeline"></div>' .
		  '</div>' .
		  '<div class = "mapcontainer" style = "'.$mapcontainer.'">' .
		  '<div id = "map-'.$div_id.'" class = "map"></div>' .
		  '</div>' .
		  '</div>' .
		  '</body>';
	  $output .= '</html>';
	  
	  $directory = file_directory_path().'/timelinemap_embed';
	  file_check_directory($directory, TRUE);
	  $directory = file_directory_path().'/timelinemap_embed/' . $user->uid;
	  file_check_directory($directory, TRUE);
	  if(empty($_POST['overwrite'])){
		$filename = $user->name . '_timelinemap_embed' . '.html';
		$filepath = file_save_data($output , $directory . '/' . $filename, FILE_EXISTS_RENAME);
		$update = FALSE;
	  } else{
		$filename = $_POST['overwrite'] . '.html';
		$filepath = file_save_data($output, $directory . '/' . $filename, FILE_EXISTS_REPLACE);
		$update = TRUE;
	  }
	  
	  $result = "<div class=\"embed-instructions\"><strong>Embed link:</strong><br/>";
	  $result .= "<div style = 'border: 1px solid #cdcdcd; padding: 3px; color: #666;' class=\"embed-code\" >";
	  $result .= "&lt;iframe scrolling = \"no\" width=\"" . $_POST['attrwidth'] . "\" height=\"" . $_POST['attrheight'] . "\" src=\"";
	  $result .= $base_url . "/";
	  $result .= $filepath;
	  $result .= "\"&gt;&lt;/iframe&gt</div></div>";
	  _timelinemap_save_file_data($user->uid, $filename, $filepath, $_POST['storage'], $_POST['vidiLink'], $_POST['viewLink'], $_POST['datasource'], $result, $update, $_POST['title']);
	  echo $result;
  } 
}

/**
 * Submit handler for the embed administration form.
 * @param $form
 * @param $form_state
 */
function timelinemap_my_visualizations_submit($form, &$form_state) {
  if(isset($form_state['values']['embeds_all']['embeds_to_delete'])){
    $for_deleting = explode(',', $form_state['values']['embeds_all']['embeds_to_delete']); 
 
    foreach($for_deleting as $value){
      $filepath = db_result(db_query("SELECT filepath FROM {files} WHERE fid=%d", $value));
      $sql = "DELETE FROM {timelinemap_embeds} WHERE fid = %d";
      if(!db_query($sql, $value)){
        drupal_set_message(t('An error occured while deleting the record with fid @id from the timelinemap_embeds table!', array('@id' => $value)), 'error');
        return TRUE;
      }
      $sql = "DELETE FROM {files} WHERE fid = %d";
      if(!db_query($sql, $value)){
        drupal_set_message(t('An error occured while deleting the record with fid @id  from the files table!', array('@id' => $value)), 'error');
        return TRUE;
      }
      if(!file_delete($filepath)){
        drupal_set_message(t('The file @path was NOT deleted!', array('@path' => $filepath)), 'error');
        return TRUE;
      }  
    }
    drupal_set_message(t('The file @path was deleted successfully!', array('@path' => $filepath)), 'status');
    return TRUE;   
  }
  $form_state['rebuild'] = TRUE; 
} 
