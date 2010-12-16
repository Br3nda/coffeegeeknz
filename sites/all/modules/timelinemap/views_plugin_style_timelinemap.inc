<?php
// $Id: views_plugin_style_timelinemap.inc,v 1.3 2010/07/14 14:55:16 vesnaradivojevic Exp $

/**
 * @file views_plugin_style_timelinemap.inc
 */

/**
 * Style plugin to render each item on Timelinemap.
 *
 * @ingroup views_style_plugins
 */
class views_plugin_style_timelinemap extends views_plugin_style {
  /**
   * Set default options
   */
  function option_definition() {
    drupal_add_tabledrag('arrange', 'order', 'sibling', 'weight');
    $options = parent::option_definition();
    $options['bands']['contains'] = array(
      'focus' => array('default' => 'today'),
      'band1_unit' => array('default' => 'day'),
      'band2_unit' => array('default' => 'month'),
    );
    $options['datasource'] = array(
      'default' => 'basic',
    );
    return $options;
  }
  
  
  /**
   * Render option form.
   */
  function options_form(&$form, &$form_state) { 
    parent::options_form($form, $form_state);
	$defaults = array(
      'baselayers' => array(
        'map' => array(
          'title' => t('Map: Standard street map.'),
          'default' => TRUE,
        ),
        'satellite' => array(
          'title' => t('Satellite: Standard satellite map.'),
          'default' => TRUE,
        ),
        'hybrid' => array(
          'title' => t('Hybrid: Hybrid satellite map.'),
          'default' => TRUE,
        ),
        'physical' => array(
          'title' => t('Terrain: Physical feature map.'),
          'default' => TRUE,
        )
	  )	
    );
    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#value' => t('You need at least one field before you can configure your field settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>',
      );
    }
    else { 
      $field_names = array('' => '--');
      $date_field_names = array('' => '--');
      $iconimage_field_names = array('' => '--');
      //$location_field_names = array('' => '--');

      //supported format for date and image
      $supported_date_handlers = array(
        'views_handler_field_date',
        'date_handler_field_multiple',
      );
      $supported_iconimage_handlers = array(
        'content_handler_field_multiple',
        'views_handler_field_taxonomy_image',
      );
      /* $supported_location_handlers = array(
        'location_views_handler_field_latitude',
	    'location_views_handler_field_longitude'
      ); */
      foreach ($handlers as $field => $handler) {
        $field_names[$field] = $handler->ui_name();
        if ($label = $handler->label()) {
          $field_names[$field] .= " (\"$label\")";
        }
        /* if (in_array(get_class($handler), $supported_location_handlers)) {
          $location_field_names[$field] = $field_names[$field];
        } */
        if (in_array(get_class($handler), $supported_date_handlers)) {
          $date_field_names[$field] = $field_names[$field];
        }
        if (in_array(get_class($handler), $supported_iconimage_handlers)) {
          if (get_class($handler) == 'views_handler_field_taxonomy_image') {
            $iconimage_field_names[$field] = $field_names[$field];
          }
          elseif ($handler->content_field['widget']['module'] == 'imagefield') {
            $iconimage_field_names[$field] = $field_names[$field];
          }
        }
      }
	  
      //display
      $form['display'] = array('#type' => 'fieldset', '#title' => t('Display settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
      $form['display']['width'] = array(
        '#type' => 'textfield',
        '#title' => t('Width'),
        '#default_value' => isset($this->options['display']['width']) ? $this->options['display']['width'] : variable_get('timelinemap_default_width', '100%'),
        '#size' => 6,
        '#maxlength' => 6,
        '#description' => t('The width of the timeline and map (in units of em, px or %), e.g. 600px or 90%. Leave blank to use default value.'),
        '#required' => TRUE
      );
      $form['display']['tlheight'] = array(
        '#type' => 'textfield',
        '#title' => t('Timeline height'),
        '#default_value' => isset($this->options['display']['tlheight']) ? $this->options['display']['tlheight'] : variable_get('timelinemap_default_tlheight', '400px'),
        '#size' => 6,
        '#maxlength' => 6,
        '#description' => t('The height of the timeline (in units of em or px), e.g. 400px. Leave blank to use default value.'),
        '#required' => TRUE,
      );
      $form['display']['mheight'] = array(
        '#type' => 'textfield',
        '#title' => t('Map height'),
        '#default_value' => isset($this->options['display']['mheight']) ? $this->options['display']['mheight'] : variable_get('timelinemap_default_mheight', '400px'),
        '#size' => 6,
        '#maxlength' => 6,
        '#description' => t('The height of the map (in units of em or px), e.g. 400px. Leave blank to use default value.'),
        '#required' => TRUE
      );
	  $form['display']['embed_link'] = array(
        '#type' => 'checkbox',
        '#weight' => 4,
        '#title' => t('Display embed link'),
        '#return_value' => 1,
        '#default_value' => isset($this->options['display']['embed_link']) ? $this->options['display']['embed_link'] : '0',
      );	 
	  
	  
      $form['display']['layers'] = array(
        '#type' => 'fieldset',
        '#title' => t('Add additional layers and overlays to the map'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => t('Here you can add additional layers and/or overlays to the map. You have to define them ahead of time on the !administration page',
                           array('!administration' => user_access('administer site configuration') ? l('administration', 'admin/settings/timelinemap/layers/addlayer', array('attributes' => array('target' => '_blank'))) : 'administration')),    
    );
      $results = db_query("SELECT * FROM {timelinemap_layers}");
      $layers = array(); $overlays = array('' => 'None');
      $default_layers = array();
      while ($result = db_fetch_object($results)) {
        if ($result->baselayer) {
          $layers[$result->lid] = $result->title;
          $default_layers[$result->name] = $result->title;
        } else
          $overlays[$result->lid] = $result->title;
      }
      $form['display']['layers']['layer'] = array(
        '#type' => 'select',
        '#title' => t('Select layers to add'),
        '#multiple' => TRUE,
        '#options' => $layers,
        '#default_value' => isset($this->options['display']['layers']['layer']) ? $this->options['display']['layers']['layer'] : '' 
      );
      $form['display']['layers']['overlay'] = array(
        '#type' => 'select',
        '#title' => t('Select an overlay to add'),
        '#options' => $overlays,
        '#default_value' => isset($this->options['display']['layers']['overlay']) ? $this->options['display']['layers']['overlay'] : '' 
      );
      foreach ($defaults['baselayers'] as $key => $value) {
        if ($value)
          $default_layers[$key] = $key;
      }
    
      $form['display']['layers']['defaultlayer'] = array(
        '#type' => 'select',
        '#title' => t('Select the default map layer'),
        '#options' => $default_layers,
        '#default_value' => isset($this->options['display']['layers']['defaultlayer']) ? $this->options['display']['layers']['defaultlayer'] : $default_layers['map']
      ); 
	  
      //timelinemap types
      $form['datasource'] = array(
        '#type' => 'select',
        '#title' => t('Timelinemap types'),
        '#options' => array(
          'basic' => t('Basic'),
          'pathline' => t('Pathline'),
        ),
        '#default_value' => $this->options['datasource'],
        '#multiple' => FALSE,
      );
      
      //fields mapping	  
      $form['fields'] = array(
        '#type' => 'fieldset',
        '#title' => t('Field usage'),
        '#collapsible' => TRUE, '#collapsed' => FALSE, 
        '#description' => t('Select the fields that contain the title, start time and end time of each item'),
      );
      $form['fields']['title'] = array(
          '#type' => 'select',
          '#title' => t('Title'),
          '#options' => $field_names,
          '#default_value' => $this->options['fields']['title'],
          '#required' => TRUE,
      );
      $form['fields']['link'] = array(
          '#type' => 'checkbox',
          '#title' => t('Link to node'),
          '#default_value' => $this->options['fields']['link'],
          '#required' => FALSE,
      );
       $form['fields']['description'] = array(
          '#type' => 'select',
          '#title' => t('Description'),
          '#options' => $field_names,
          '#default_value' => $this->options['fields']['description'],
      ); 
	  
	  // @todo: get_handlers()
	   $form['fields']['table'] = array(
		'#type' => 'item',
		'#theme' => 'timelinemap_rearrange_form',
		'#tree' => TRUE
	  );
		  
	  $count = 0;
	  $sort_array = $field_names;
	  $t = array_shift($sort_array);
      
      if (isset($this->options['fields']['table']) && count($this->options['fields']['table'] == count($field_names))){
		$sort_array = $this->options['fields']['table'];
        uasort($sort_array, '_timelinemap_element_sort');
      
        $sort_array = array_keys($sort_array);
        $tmp = array();
        foreach($sort_array as $value)
          $tmp[$value] = $value;
        $sort_array = $tmp; unset($tmp);
        foreach ($sort_array as $key => $value){
          $handler = $this->display->handler->get_handler('field', $key);
          $sort_array[$key] = $handler->ui_name();
        }
		$tmp_fields = $field_names;
		array_shift($tmp_fields);
		$diff = array_diff_assoc($tmp_fields, $sort_array);
        foreach($diff as $key => $value)
          $sort_array[$key] = $value;
		
      }
	  		
	  foreach ($sort_array as $id => $field) {
		$form['fields']['table'][$id] = array('#tree' => TRUE);
		$form['fields']['table'][$id]['weight'] = array(
		  '#type' => 'weight',
		  '#delta' => 200,
		  '#default_value' => ++$count,
		);
		$form['fields']['table'][$id]['name'] = array(
  		  '#value' => $field,
		);
		$form['fields']['table'][$id]['added'] = array(
		  '#type' => 'checkbox',
		  '#id' => 'views-added-' . $id,
		  '#attributes' => array('class' => 'views-add-checkbox'),
		  '#default_value' => isset($this->options['fields']['table'][$id]['added'])?$this->options['fields']['table'][$id]['added']:0,
		);
	  }

	  // Add javascript settings that will be added via $.extend for tabledragging
	  $form['#js']['tableDrag']['arrange']['weight'][0] = array(
		'target' => 'weight',
		'source' => NULL,
		'relationship' => 'sibling',
		'action' => 'order',
		'hidden' => TRUE,
		'limit' => 0,
	  );
	  
      $form['fields']['icon'] = array(
          '#type' => 'select',
          '#title' => t('Image'),
          '#options' => $iconimage_field_names,
          '#default_value' => $this->options['fields']['icon'],
      );

      if (module_exists('imagecache')) {
        $imagecache_presets = array('' => t('Orginal image size'));
        foreach (imagecache_presets() as $preset) {
          $imagecache_presets[$preset['presetname']] = $preset['presetname'];
        }
        $form['fields']['advanced']['imagecache_icon'] = array(
          '#type' => 'select',
          '#title' => t('Imagecache preset for Image'),
          '#options' => $imagecache_presets,
          '#default_value' => $this->options['fields']['advanced']['imagecache_icon'],
        );
      }
      //start and end date for timeline
      $form['fields']['start'] = array(
        '#type' => 'select',
        '#title' => t('Start'),
        //'#options' => $date_field_names,
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['start'],
        '#required' => TRUE,
      );

      $form['fields']['end'] = array(
        '#type' => 'select',
        '#title' => t('End'),
        //'#options' => $date_field_names,
        '#options' => $field_names,
        '#default_value' => isset($this->options['fields']['end']) ? $this->options['fields']['end'] : '',
        '#process' => array('views_process_dependency'),
        '#dependency' => array('edit-style-options-datasource' => array('basic')),
      );
        
      $form['fields']['latitude'] = array(
        '#type' => 'select',
        '#title' => 'Latitude field',
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['latitude'],
        '#required' => TRUE,
      );

      $form['fields']['longitude'] = array(
        '#type' => 'select',
        '#title' => t('Longitude field'),
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['longitude'],
        '#required' => TRUE,
      );
      //pathcolor for pathline type
      $form['pathcolor'] = array(
        '#title' => t('Path color'),
        '#type' => 'textfield',
        '#process' => array('views_process_dependency'),
        '#dependency' => array('edit-style-options-datasource' => array('pathline')),
        '#default_value' => isset($this->options['pathcolor']) ? $this->options['pathcolor'] : '',
        '#description' => t('Enter one hexadecimal RGB color value per line. Examples: #FF00FF #FF0099 #998811')
      );
	  
	  $form['colors']['values'] = array(
          '#type' => 'select',
          '#title' => t('Color coding'),
          '#options' => array('custom' => 'Custom', 'city_funky' => 'City Funky', 'pure_classic' => 'Pure Classic', 'ecology_green' => 'Ecology Green', 'clear_blue' => 'Clear Blue', 'lavender' => 'Lavender', 'burgundy_red' => 'Burgundy Red', 'orange_sun' => 'Orange Sun', 'vidi_special' => 'VIDI Special'),
          '#default_value' => isset($this->options['colors']['values']) ? $this->options['colors']['values'] : '',
      );
	  $form['colors']['custom_values'] = array(
          '#type' => 'select',
		  '#multiple' => TRUE,
          '#title' => t('Select colors'),
          '#options' => array('yellow' => 'yellow', 'green' => 'green', 'light blue' => 'light blue', 'orange' => 'orange', 'purple' => 'purple', 'blue' => 'blue', 'red' => 'red'),
		  '#default_value' => isset($this->options['colors']['custom_values']) ? $this->options['colors']['custom_values'] : '',
		  '#process' => array('views_process_dependency'),
          '#dependency' => array('edit-style-options-colors-values' => array('custom')),
      );
	  
      // Band/interval settings
      $form['bands'] = array('#type' => 'fieldset', '#title' => t('Band/interval settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
      $intervals = array('SECOND' => t('Second'), 'MINUTE' => t('Minute'), 'HOUR' => t('Hour'), 'DAY' => t('Day'), 'WEEK' => t('Week'), 'MONTH' => t('Month'), 'YEAR' => t('Year'), 'DECADE' => t('Decade'), 'CENTURY' => t('Century'), 'MILLENNIUM' => t('Millennium'));
      $form['bands']['band1_unit'] = array(
         '#type' => 'select',
         '#title' => t('Main band interval unit'),
         '#default_value' => $this->options['bands']['band1_unit'],
         '#options' => $intervals,
         '#description' => t(''),
      );
      $form['bands']['band2_unit'] = array(
         '#type' => 'select',
         '#title' => t('Summary band interval unit'),
         '#options' => array_merge(array('' => t('Not displayed')), $intervals),
		 '#default_value' => $this->options['bands']['band2_unit'],
         '#description' => t(''),
      );
    }
  }
  
  /**
  * Simple submit handler
  */
  function options_submit(&$form, &$form_state) {
     if ($form_state['values']['style_options']['datasource'] == 'pathline') { 
      unset($form_state['values']['style_options']['fields']['end']);
     }
   }
   
  /**
  * Render the given style.
  */    
  function render() {
   // Load only when neccessary
    module_load_include('inc', 'timelinemap', 'classes/Timelinemap.class');
    module_load_include('inc', 'timelinemap', 'classes/TimelinemapEvent.class');

    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
	
    // Build data for timelinemap
    $timelinemap = new Timelinemap($this, $sets); 
	
    // Convert data to json and push them to drupal's js behaviors
    $timelinemap->timelinemap_to_json();

    // options that are used for template preprocessor
    $this->options['timelinemap'] = array(
      'name' => get_class($this->view->display_handler),
      'class' => $this->view->name,
	  'embed_link' => $this->view->style_options['display']['embed_link'],
      'width' => _timelinemap_width_to_dim($this->view->style_options['display']['width']),
      'tlheight' => _timelinemap_to_dim($this->view->style_options['display']['tlheight']),
      'mheight' => _timelinemap_to_dim($this->view->style_options['display']['mheight']),
    );
    return theme($this->theme_functions(), $this->view, $this->options);
  }
}

