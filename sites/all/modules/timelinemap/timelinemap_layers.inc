<?php
// $Id: timelinemap_layers.inc,v 1.1 2010/07/15 07:08:22 vesnaradivojevic Exp $

/**
 * @file
 *
 */
 
/**
 * Ahah callback foe add and edit layer functions
 */
function timelinemap_add_layer_js() {
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];

  $form = form_get_cache($form_build_id, $form_state);

  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id); 

  $preset_colors = $form['layer']['ahah_wrapper']['options'];
  unset($preset_colors['#prefix'], $preset_colors['#suffix']);
  $output = theme('status_messages') . drupal_render($preset_colors);

  drupal_json(array('status' => TRUE, 'data' => $output));  
}

/**
 * Add layer form
 * @param $form_state
 */
function timelinemap_add_new_layer_form($form_state) {
  //If the form fail validation, remember the #action attribute
  //so it doesn't redirect to the ahah callback
  if (!isset($form_state['storage']['old_action']))
    $form_state['storage']['old_action'] = isset($form['#action']) ? $form['#action'] : request_uri();  
    
  // If the form action change, restore the old one.
  if (isset($form_state['storage']['old_action']) && ($form['#action']!=$form_state['storage']['old_action'])) {
    $form['#action'] = $form_state['storage']['old_action'];
  }       
  
  $form['layer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new layer'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,      
  );
  
  $form['layer']['name'] = array(
    '#title' => t('Entern the name of the layer'),
    '#type' => 'textfield',
    '#attributes' => array('style' => 'background:#fbf693;'),
    '#description' => t('The name <strong>must</strong> be entered as it is on the provider site!<br/> Currently the only suported provider is !mapbox. Get their layers from !here.', 
                        array('!mapbox' => l('MapBox', 'http://mapbox.com', array('attributes' => array('target' => '_blank'))), '!here' => l('here', 'http://mapbox.com/tiles', array('attributes' => array('target' => '_blank'))))),
    '#maxlength' => 255,
    '#default_value' => isset($form_state['values']['name'])?$form_state['values']['name']:''    
  );
  $form['layer']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter the title of the layer'),
    '#attributes' => array('style' => 'background:#fbf693;'),
    '#maxlength' => 255,
    '#default_value' => isset($form_state['values']['title'])?$form_state['values']['title']:''    
  );
 
  $form['layer']['baselayer'] = array(
    '#type' => 'select',
    '#default_value' => isset($form_state['values']['baselayer']) ? $form_state['values']['baselayer'] : 1,
    '#title' => t('Is baselayer?'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#ahah' => array(
      'path'    => 'timelinemap/add_layer_js',
      'wrapper' => 'timelinemap-layer-options',
      'method' => 'replace',
      'effect' => 'fade'
    ), 
  ); 
  
  $form['layer']['ahah_wrapper'] = array(
    '#type' => 'fieldset',
    '#attributes' => array('id' => 'ahah-wrapper'),
    '#title' => t('Layer options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['layer']['ahah_wrapper']['options'] = array(
    '#tree' => TRUE,
    '#prefix' => '<div id="timelinemap-layer-options">',
    '#suffix' => '</div>',
  );  

  if (isset($form_state['values']['baselayer']) && !$form_state['values']['baselayer']) {
    $form['layer']['ahah_wrapper']['options']['minZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['minZoom']) ? $form_state['values']['options']['minZoom'] : 0,
      '#title' => t('Select minimum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );  
    
    $form['layer']['ahah_wrapper']['options']['maxZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['maxZoom']) ? $form_state['values']['options']['maxZoom'] : 19,
      '#title' => t('Select maximum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );  
      
    $form['layer']['ahah_wrapper']['options']['type'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['type']) ? $form_state['values']['options']['type'] : 'png',
      '#title' => t('The type of the overlay'),
      '#options' => array('png' => 'png', 'jpg' => 'jpg')     
    );
    
    $form['layer']['ahah_wrapper']['options']['opacity'] = array(
      '#type' => 'textfield',
      '#tree' => TRUE,
      '#attributes' => array('style' => 'background:#fbf693;'),
      '#default_value' => isset($form_state['values']['options']['opacity']) ? $form_state['values']['options']['opacity'] : '1.0',
      '#title' => t('The opacity of the overlay'),
      '#size' => 4,
      '#maxlength' => 4,      
    );    
  }
  else {
    $form['layer']['ahah_wrapper']['options']['minZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['minZoom']) ? $form_state['values']['options']['minZoom'] : 0,
      '#title' => t('Select minimum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );  
    
    $form['layer']['ahah_wrapper']['options']['maxZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['maxZoom']) ? $form_state['values']['options']['maxZoom'] : 19,
      '#title' => t('Select maximum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );      
  }
  
  $form['layer']['timelinemap_save_layer_dummy'] = array(
    '#type' => 'submit',
    '#value' => t('Save layer'),
    '#validate' => array(TRUE),
    '#attributes' => array('style' => 'display: none;')    
  );  
    
  $form['layer']['timelinemap_save_layer'] = array(
    '#type' => 'submit',
    '#validate' => array('timelinemap_add_new_layer_form_validate'),
    '#submit' => array('timelinemap_add_new_layer_save'),
    '#value' => t('Save layer'),
  ); 

  return $form; 
}

/**
 * Validation function for add new layer form
 * @param $form
 * @param &$form_state
 */
function timelinemap_add_new_layer_form_validate($form, &$form_state) {
  $field_value = trim($form_state['values']['name']);
  if (empty($field_value)) 
    form_set_error('name', t('The layer name is required.'));

  $field_value = trim($form_state['values']['title']);
  if (empty($field_value)) 
    form_set_error('title', t('The layer title is required.'));

  if (!$form_state['values']['baselayer']) {
    if (is_numeric($form_state['values']['options']['opacity']) && substr($form_state['values']['options']['opacity'], 0, 2) != '0x') {
      $field_value = (float) $form_state['values']['options']['opacity'];
      if ($field_value < 0.0 || $field_value > 1.0)
        form_set_error('options][opacity', t('The opacity must be between 0.0 and 1.0.'));
    }
    else{
      form_set_error('options][opacity', t('Invalid value.'));
    }
  }
  if (db_result(db_query("SELECT COUNT(*) FROM {timelinemap_layers} WHERE name='%s'", trim($form_state['values']['name']))) != 0) {
    form_set_error('name', t('The layer with the name !name already exists.', array('!name' => $form_state['values']['name'])));
  }  
}

/**
 * Submit handler for add new layer form
 * @param $form
 * @param &$form_state
 */
function timelinemap_add_new_layer_save($form, &$form_state) {
  $layer = new stdClass();
  $layer->name = trim($form_state['values']['name']);
  $layer->title = $form_state['values']['title'];
  $layer->baselayer = $form_state['values']['baselayer'];
  $options = $form_state['values']['options'];
  if (!$form_state['values']['baselayer'])
    $options['overlay'] = 'true';
  $layer->options = serialize($options);
  if (!drupal_write_record('timelinemap_layers', $layer)) {
    drupal_set_message(t('Error writing the layer @layer in the database!', array('@layer' => $layer->title)), 'error');
    return;
  }
  drupal_set_message(t('The layer @layer was saved successfully!', array('@layer' => $layer->title)), 'status');
}

/**
 * Submit handler for add new layer form (called only if ahah failes)
 * @param $form
 * @param &$form_state
 */
function timelinemap_add_new_layer_form_submit($form, &$form_state) {
  form_execute_handlers('validate', $form, $form_state);
  $form_state['rebuild'] = TRUE;  
}

/**
 * Controller that alternates between the list of layers and
 * the delete confirmation form
 * @param $form_state
 */
function timelinemap_layer_list_controller($form_state) {
  $checked = FALSE;
  if (!empty($form_state['values']['layers'])) {
    foreach ($form_state['values']['layers'] as $value) {
      if ($value) {
        $checked = TRUE;
        break;
      }
    }
  }
  if ($form_state['values']['op'] == 'Delete selected' && $checked) 
    return _timelinemap_layer_delete_confirm($form_state);
    
  return _timelinemap_layer_list_form($form_state);     
}

/**
 * Form thet lists existing layers
 * @param $form_state
 */
function _timelinemap_layer_list_form($form_state) {
  $limit = 20;
  $sql = 'SELECT lid, name, title, baselayer FROM {timelinemap_layers} ORDER BY name';
  $results = pager_query($sql, $limit, 0);
  $layers = array();
  while ($result = db_fetch_object($results)) {
    $layers[$result->lid] = '';
    $form['data']['name'][$result->lid] = array('#value' => $result->name);
    $form['data']['title'][$result->lid] =  array('#value' => $result->title);
    $form['data']['baselayer'][$result->lid] =  array('#value' => empty($result->baselayer) ? 'No' : 'Yes');
    $form['data']['operations'][$result->lid] = array('#value' => l('edit', 'admin/settings/timelinemap/layers/edit/' . $result->lid) . ' | ' . l('export', 'admin/settings/timelinemap/layers/export/' . $result->lid));
  }   
  $form['data']['layers'] = array('#type' => 'checkboxes', '#options' => $layers);
  $form['data']['pager'] = array('#value' => theme('pager', NULL, $limit, 0));
  $form['data']['#theme'] = 'timelinemap_list_layers';
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete selected'),    
  );  
  return $form;  
}

/**
 * Confirmation form for deleting a layer
 * @param $form_state
 */
function _timelinemap_layer_delete_confirm($form_state) {
  $desc = 'The selected layers vill be deleted from the database. This action CAN NOT be undone.';
  $form['process'] = array('#type' => 'hidden', '#value' => 'true');
  $form['destination'] = array('#type' => 'hidden', '#value' => 'admin/settings/timelinemap/layers');
  $to_delete = array();
  foreach ($form_state['values']['layers'] as $key => $value) {
    if ($value) {
      $to_delete[]=$value;
    }
  }
  $form['layers_to_delete'] = array('#type' => 'hidden', '#value' => implode(',', $to_delete));
  return confirm_form($form,
                      t('Are you sure you want to delete the selected layers?'),
                      'admin/settings/timelinemap/layers',
                      $desc,
                      'Delete',
                      'Return');   
}

/**
 * Submit handler for that hadles the deleting of layer(s)
 * @param $form
 * @param &$form_state
 */
function timelinemap_layer_list_controller_submit($form, &$form_state) {
  if (isset($form_state['values']['layers_to_delete'])) {
    $for_deleting = explode(',', $form_state['values']['layers_to_delete']);
    foreach ($for_deleting as $value) {
      if (!db_query("DELETE FROM {timelinemap_layers} WHERE lid = %d", $value)) {
        drupal_set_message(t('An error occured while deleting the one of the selected layers!'), 'error');
        return TRUE;
      }             
    }
    drupal_set_message(t('The selected layers were deleted successfully!'), 'status');
    return TRUE;        
  }  
  $form_state['rebuild'] = TRUE;
}

/**
 * Theme function for the layer list
 * @param $form
 */
function theme_timelinemap_list_layers($form) {
  $select_header = theme('table_select_header_cell');
  $header = array(
    $select_header,
    t('Layer name'),
    t('Layer title'),
    t('Is baselayer'),
    t('Operation'),
  );
  
  $output = '';
  $rows = array();
  $counter = 0;
  foreach (element_children($form['name']) as $key) {
    $row = array();
    $row[] = drupal_render($form['layers'][$key]);
    $row[] = drupal_render($form['name'][$key]);
    $row[] = drupal_render($form['title'][$key]);
    $row[] = drupal_render($form['baselayer'][$key]);    
    $row[] = drupal_render($form['operations'][$key]);
    $rows[] = $row;
    $counter++;
  }
  if ($counter == 0) {
    $rows[] = array(array('data' => t('No posts available.'), 'colspan' => '5'));
  }

  $output .= theme('table', $header, $rows);
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  $output .= drupal_render($form);

  return $output;    
}

/**
 * Edit existing layer form
 * @param $form_state
 */
function timelinemap_layer_edit_form($form_state) {
  if (!isset($form_state['values'])) {
    $layer = db_fetch_object(db_query("SELECT * FROM {timelinemap_layers} WHERE lid=%d", arg(5)));
    $form_state['values']['name'] = $layer->name;
    $form_state['values']['lid'] = $layer->lid;
    $form_state['values']['title'] = $layer->title;
    $form_state['values']['baselayer'] = $layer->baselayer;
    $form_state['values']['options'] = unserialize($layer->options);
  }
  
  if (!isset($form_state['storage']['old_action']))
    $form_state['storage']['old_action'] = isset($form['#action']) ? $form['#action'] : request_uri();  
    
  // If the form action change, restore the old one.
  if (isset($form_state['storage']['old_action']) && ($form['#action']!=$form_state['storage']['old_action'])) {
    $form['#action'] = $form_state['storage']['old_action'];
  }     
  
  $form['#redirect'] = 'admin/settings/timelinemap/layers';
  
  $form['lid'] = array(
    '#type' => 'value',
    '#value' => $form_state['values']['lid']
  );
    
  $form['layer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new layer'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,      
  );
  
  $form['layer']['name'] = array(
    '#title' => t('Entern the name of the layer'),
    '#type' => 'textfield',
    '#description' => t('The name <strong>must</strong> be entered as it is on the provider site!<br/> Currently the only suported provider is !mapbox. Get their layers from !here.', 
                        array('!mapbox' => l('MapBox', 'http://mapbox.com', array('attributes' => array('target' => '_blank'))), '!here' => l('here', 'http://mapbox.com/tiles', array('attributes' => array('target' => '_blank'))))),
    '#attributes' => array('style' => 'background:#fbf693;'),
    '#maxlength' => 255,
    '#default_value' => isset($form_state['values']['name'])?$form_state['values']['name']:''    
  );
  $form['layer']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Entern the title of the layer'),
    '#attributes' => array('style' => 'background:#fbf693;'),
    '#maxlength' => 255,
    '#default_value' => isset($form_state['values']['title'])?$form_state['values']['title']:''    
  );
 
  $form['layer']['baselayer'] = array(
    '#type' => 'select',
    '#default_value' => isset($form_state['values']['baselayer']) ? $form_state['values']['baselayer'] : 1,
    '#title' => t('Is baselayer?'),
    '#options' => array(0 => 'No', 1 => 'Yes'),
    '#ahah' => array(
      'path'    => 'timelinemap/add_layer_js',
      'wrapper' => 'timelinemap-layer-options',
      'method' => 'replace',
      'effect' => 'fade'
    ), 
  ); 
  
  $form['layer']['ahah_wrapper'] = array(
    '#type' => 'fieldset',
    '#attributes' => array('id' => 'ahah-wrapper'),
    '#title' => t('Layer options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['layer']['ahah_wrapper']['options'] = array(
    '#tree' => TRUE,
    '#prefix' => '<div id="timelinemap-layer-options">',
    '#suffix' => '</div>',
  );  

  if (isset($form_state['values']['baselayer']) && !$form_state['values']['baselayer']) {
    $form['layer']['ahah_wrapper']['options']['minZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['minZoom']) ? $form_state['values']['options']['minZoom'] : 0,
      '#title' => t('Select minimum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );  
    
    $form['layer']['ahah_wrapper']['options']['maxZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['maxZoom']) ? $form_state['values']['options']['maxZoom'] : 19,
      '#title' => t('Select maximum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );  
      
    $form['layer']['ahah_wrapper']['options']['type'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['type']) ? $form_state['values']['options']['type'] : 'png',
      '#title' => t('The type of the overlay'),
      '#options' => array('png' => 'png', 'jpg' => 'jpg')     
    );
    
    $form['layer']['ahah_wrapper']['options']['opacity'] = array(
      '#type' => 'textfield',
      '#attributes' => array('style' => 'background:#fbf693;'),
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['opacity']) ? $form_state['values']['options']['opacity'] : '1.0',
      '#title' => t('The opacity of the overlay'),
      '#size' => 4,
      '#maxlength' => 4,      
    );    
  }
  else {
    $form['layer']['ahah_wrapper']['options']['minZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['minZoom']) ? $form_state['values']['options']['minZoom'] : 0,
      '#title' => t('Select minimum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );  
    
    $form['layer']['ahah_wrapper']['options']['maxZoom'] = array(
      '#type' => 'select',
      '#tree' => TRUE,
      '#default_value' => isset($form_state['values']['options']['maxZoom']) ? $form_state['values']['options']['maxZoom'] : 19,
      '#title' => t('Select maximum layer zoom level'),
      '#options' => array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19) 
    );      
  }
  
  $form['layer']['timelinemap_save_layer_dummy'] = array(
    '#type' => 'submit',
    '#value' => t('Save layer'),
    '#validate' => array(TRUE),
    '#attributes' => array('style' => 'display: none;')    
  );  
    
  $form['layer']['timelinemap_save_layer'] = array(
    '#type' => 'submit',
    '#validate' => array('timelinemap_edit_layer_form_validate'),
    '#submit' => array('timelinemap_edit_layer_save'),
    '#value' => t('Save layer'),
  ); 
  
  $form['layer']['timelinemap_cancel'] = array(
    '#type' => 'submit',
    '#submit' => array('timelinemap_edit_layer_save'),
    '#value' => t('Cancel'),
  );    

  return $form; 
}

/**
 * Validation function for edit layer form
 * @param $form
 * @param &$form_state
 */
function timelinemap_edit_layer_form_validate($form, &$form_state) {
  $field_value = trim($form_state['values']['name']);
  if (empty($field_value)) 
    form_set_error('name', t('The layer name is required.'));

  $field_value = trim($form_state['values']['title']);
  if (empty($field_value)) 
    form_set_error('title', t('The layer title is required.'));

  if (!$form_state['values']['baselayer']) {
    if (is_numeric($form_state['values']['options']['opacity']) && substr($form_state['values']['options']['opacity'], 0, 2) != '0x') {
      $field_value = (float) $form_state['values']['options']['opacity'];
      if ($field_value < 0.0 || $field_value > 1.0)
        form_set_error('options][opacity', t('The opacity must be between 0.0 and 1.0.'));
    }
    else {
      form_set_error('options][opacity', t('Invalid value.'));
    }
  } 
}

/**
 * Submit handler for edit layer form (updates the layer in the database)
 * @param $form
 * @param &$form_state
 */
function timelinemap_edit_layer_save($form, &$form_state) {
  if ($form_state['clicked_button']['#id'] == 'edit-timelinemap-cancel') {
    unset($form_state['values']);
    unset($form_state['storage']);
  } 
  elseif ($form_state['clicked_button']['#id'] == 'edit-timelinemap-save-layer') {
    $layer = new stdClass();
    $layer->name = $form_state['values']['name'];
    $layer->title = $form_state['values']['title'];
    $layer->lid = $form_state['values']['lid'];
    $layer->baselayer = $form_state['values']['baselayer'];
    $options = $form_state['values']['options'];
    if (!$form_state['values']['baselayer'])
      $options['overlay'] = 'true';
    $layer->options = serialize($options);
    if (!drupal_write_record('timelinemap_layers', $layer, 'lid')) {
      drupal_set_message(t('Error writing the layer @layer in the database!', array('@layer' => $layer->title)), 'error');
      return;
    }
    drupal_set_message(t('The layer @layer was changed successfully!', array('@layer' => $layer->title)), 'status');
    unset($form_state['values']);
    unset($form_state['storage']); 
  }
}

/**
 * Form that showsthe exported layer
 * @param $form_state
 */
function timelinemap_layer_export_form($form_state) {
  $layer = db_fetch_object(db_query("SELECT * FROM {timelinemap_layers} WHERE lid = %d", arg(5)));
  $lines = 2;
  $export = '$layer = array(' . "\n";
  $export .= "  'name' => '" . $layer->name . "',\n"; $lines++;
  $export .= "  'title' => '" . $layer->title . "',\n"; $lines++;
  $export .= "  'baselayer' => '" . $layer->baselayer . "',\n"; $lines++;
  $options = unserialize($layer->options);
  $export .= "  'options' => array(" . "\n";
  foreach ($options as $key => $value) {
    $export .= "    '" . $key . "' => '" . $value . "',\n"; $lines++;
  }
  $export .= "  ),\n"; $lines ++;
  $export .= ");"; $lines ++;
    
  $form['export'] = array(
    '#type' => 'textarea',
    '#default_value' => $export,
    '#rows' => $lines,
  );
  $form['timelinemap_return'] = array(
    '#value' => l('Return', 'admin/settings/timelinemap/layers'),
  );  
  return $form;
}

/**
 * Import layer form
 * @param $form_state
 */
function timelinemap_import_layer_form($form_state) {
  $form['layer'] = array(
    '#type' => 'textarea',
    '#title' => t('Paste layer array here.'),
    '#description' => t("Import previously exported layer. The syntax must be valid if the layer is to be imported."),
    '#required' => TRUE,
  );  
  $form['timelinemap_import'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );  
  $form['timelinemap_return'] = array(
    '#value' => l('Return', 'admin/settings/timelinemap/layers'),
  );  
  return $form;  
}

/**
 * Import layer form validation function
 * @param $form
 * @param &$form_state
 */
function timelinemap_import_layer_form_validate($form, &$form_state) {
  $layer = NULL;
  @eval($form_state['values']['layer']);
  if (!isset($layer) || !is_array($layer)) {
    form_set_error('layer', t('The import data is not valid import text.'));
    return;
  } 
  $keys = array_keys($layer);
  if (!in_array('name', $keys) || !in_array('title', $keys) || !in_array('baselayer', $keys) || !in_array('options', $keys)) {
    form_set_error('layer', t('The keys of the $layer array are not named correctly.<br/> The keys must be names \'name\', \'title\', \'baselayer\' and \'options\'.'));
    return;
  }
  if ($layer['baselayer'] != '0' && $layer['baselayer'] != '1') {
    form__set_error('layer', t('The \'baselayer\' element of the $layer array must have a value of 0 or 1.'));
  }
  if (!is_array($layer['options'])) {
    form__set_error('layer', t('The \'options\' element of the $layer array must also be an array.'));
    return;
  }
  $keys = array_keys($layer['options']);
  switch ($layer['baselayer']) {
    case '0':
      if (!in_array('minZoom', $keys) || !in_array('maxZoom', $keys) || !in_array('type', $keys) || !in_array('opacity', $keys) || !in_array('overlay', $keys)) {
        form_set_error('layer', t('The keys of the $layer[\'options\'] array are not named correctly.<br/> The keys must be names \'minZoom\', \'maxZoom\', \'type\', \'opacity\' and \'overlay\'.'));
        return;
      }    
      break;
    case '1': 
      if (!in_array('minZoom', $keys) || !in_array('maxZoom', $keys)) {
        form_set_error('layer', t('The keys of the $layer[\'options\'] array are not named correctly.<br/> The keys must be names \'minZoom\'and \'maxZoom\'.'));
        return;
      }    
  }
  if ($layer['options']['minZoom'] > 19 || $layer['options']['minZoom'] < 0)
    form_set_error('layer', t('The value of $layer[\'options\'][\'minZoom\'] element must be berween 0 and 19.'));
    
  if ($layer['options']['maxZoom'] > 19 || $layer['options']['maxZoom'] < 0)
    form_set_error('layer', t('The value of $layer[\'options\'][\'maxZoom\'] element must be berween 0 and 19.'));
    
  if ($layer['baselayer'] == '0') {
    if ($layer['options']['type'] != 'jpg' && $layer['options']['type'] != 'png')
      form_set_error('layer', t('The value of $layer[\'options\'][\'type\'] element must be \'jpg\' or \'png\'.')); 
            
    if (is_numeric($layer['options']['opacity']) && substr($layer['options']['opacity'], 0, 2) != '0x') {
      $opacity = (float) $layer['options']['opacity'];
      if ($opacity < 0.0 || $opacity > 1.0)
        form_set_error('layer', t('The value of $layer[\'options\'][\'type\'] element must be between 0.0 and 1.0.'));
    }
    else {
      form_set_error('options][opacity', t('Invalid value of the $layer[\'options\'][\'type\'] element.'));
    }
    if ($layer['options']['overlay'] != 'true')
      form_set_error('layer', t('The value of $layer[\'options\'][\'overlay\'] element must be \'true\' in case that the layer is not a baselayer.'));
  }
  
  if (db_result(db_query("SELECT COUNT(*) FROM {timelinemap_layers} WHERE name='%s'", trim($layer['name']))) != 0) {
    form_set_error('layer', t('The layer with the name !name already exists.', array('!name' => $layer['name'])));
  }   
}

/**
 * Import layer form submit handler
 * @param $form
 * @param &$form_state
 */
function timelinemap_import_layer_form_submit($form, &$form_state) {
  $layer = NULL;
  @eval($form_state['values']['layer']);  
  $layers = new stdClass();
  $layers->name = trim($layer['name']);
  $layers->title = $layer['title'];
  $layers->baselayer = $layer['baselayer'];
  $options = $layer['options'];
  if (!$layer['baselayer'])
    $options['overlay'] = 'true';
  $layers->options = serialize($options);
  if (!drupal_write_record('timelinemap_layers', $layers)) {
    drupal_set_message(t('Error writing the layer @layer in the database!', array('@layer' => $layers->title)), 'error');
    return;
  }
  drupal_set_message(t('The layer @layer was saved successfully!', array('@layer' => $layers->title)), 'status');  
}