<?php
// $Id: TimelinemapEvent.class.inc,v 1.3 2010/07/14 14:55:16 vesnaradivojevic Exp $

/**
 * @file TimelinemapEvent.class.inc
 * Class TimelinemapEvent
 */

/**
 *
 */
class TimelinemapEvent {
  var $view;
  var $row;
  var $data;
  var $dateformat;
  var $color;
  var $css_selector;

 /**
   * Constructor.
   *
   * @param $view
   */
  function __construct(&$view, &$row, $dateformat, $css_selector, $color) { 
    $this->view = $view;
    $this->dateformat = $dateformat;
    $this->color = $color; 
    $this->css_selector = $css_selector; 
    $this->data = $this->buildData($row);
  }

  /**
   *
   */
  function getData() { 
    return $this->data;
  }

  /**
   *
   */
  function buildData($row) {
    if (!empty($this->view->result)) {
      $tmp = $this->getDefinedFieldUsage('start');
      if (isset($row->{$tmp['alias']})) {
        $data = array();

        $tmp = $this->getDefinedFieldUsage('title');
        if (isset($tmp['alias']))
            $data['title'] = theme('timelinemap_print_title', $row, $tmp['alias']);

        $data['link'] = theme('timelinemap_print_url', $row, $this->view->style_options['fields']['link']);

        $tmp = $this->getDefinedFieldTableUsage();
		$data['descriptiontext_new'] = array();
        foreach($tmp as $key => $value){
		  if (isset($value['alias']))
            $data['descriptiontext_new'][$value['key']] = $row->$value['alias'];
		}
		
		
		
        $tmp = $this->getDefinedFieldUsage('description');
        if (isset($tmp['alias']))
            $data['descriptiontext'] = theme('timelinemap_print_title', $row, $tmp['alias']);

        $tmp = $this->getDefinedFieldUsage('latitude');
        if (isset($tmp['alias']))
            $data['latitude'] = theme('timelinemap_print_title', $row, $tmp['alias']);

        $tmp = $this->getDefinedFieldUsage('longitude');
        if (isset($tmp['alias']))
            $data['longitude'] = theme('timelinemap_print_title', $row, $tmp['alias']);
        
        $events_validated = $this->logic($row);
        $data['start'] = $events_validated['start']['formated']; 
        if (isset($events_validated['end']['formated'])) $data['end'] = $events_validated['end']['formated'];
        $data['durationEvent'] = $events_validated['durationEvent'];
        
        if (is_string($this->color)) {
          $data['color'] = $this->color;
        }

        // Support for icons on events
        if ($icon_path = $this->getIconUrl($row)) {
          $data['icon'] = $icon_path; 
        }

        // 
        $data['classname'] = $this->css_selector; 
        //$data['description'] = $this->view->style_plugin->row_plugin->render($row);
        return $data; 
      }
    }
    return FALSE;
  }


  /**
   * This function validates the dates of one event that have a relation. Like start and end date.
   * There is a logic change to timelinemap. This conversion is done here.
   *
   * @return
   *   $event: Array with the converted date and a unix timestamp on which some logic has been done
   */
  function logic($row) {

  $event = array();
    foreach (array('start', 'end') as $event_field) {

      $field1 = $this->getDefinedFieldUsage($event_field);
      
      if (isset($row->{$field1['alias']})) { 
        $field = $this->getDefinedFieldUsageDate($event_field, $row->{$field1['alias']});
        $event[$event_field]['formated'] = timelinemap_date_conversion($row->{$field['alias']}, $field['date_format'], $this->dateformat);
        $event[$event_field]['timestamp'] = timelinemap_date_conversion($row->{$field['alias']}, $field['date_format'], 'timestamp');
      }
    } 
    // Validate date values
    if (isset($event['end']) && $event['end']['timestamp'] <= $event['start']['timestamp']) {
      unset($event['end']);
    }

    if (isset($event['end'])) {
      $event['durationEvent'] = TRUE;
    }
    else {
      $event['durationEvent'] = FALSE;
    }
    return $event;
  }
  
  /**
   * The function returns the field that was defined by the views timelinemap settings.
   *
   * @param
   *   String that is expected to be "title", "start", "end", "icon"
   *
   * @return
   *   array with the following attributes set:
   *
   *   alias - The alias of the field
   *   handler - A string with the name of the handler
   *   date_format _ If it is an supported date information the expected date format
   */
  function getDefinedFieldUsage($type) {
    if ($timelinemap_settings_fields = $this->view->style_options['fields']) {
      if (isset($timelinemap_settings_fields[$type])) {
        if (is_string($timelinemap_settings_fields[$type])) {
          $fieldname = $timelinemap_settings_fields[$type];
        }
      }
      if (isset($fieldname) && $fieldname != '') {
        $field_information = array(
          'alias' => $this->view->field[$fieldname]->field_alias,
          'handler' => $this->view->field[$fieldname]->definition['handler'],
        );
        // check how to handle data from supported date fields
     /*    switch ($field_information['handler']) {
          case 'views_handler_field_date': // 1. standard views node date fields (unix timestamp)
            $field_information['date_format'] = 'timestamp';
            break;
          case 'date_handler_field_multiple': // 2. support for date module's date fields 
            $field_information['date_format'] = $this->view->field[$fieldname]->content_field['type'];
            break;
        }  */
		$field_information['date_format'] = 'timestamp';
      return $field_information; 
      }
    }
    else {
      return FALSE;
    }
  }
  
 function getDefinedFieldTableUsage() {
    $field_information = array();
    if ($timelinemap_settings_fields = $this->view->style_options['fields']) {
      if (isset($timelinemap_settings_fields['table'])) {
        if (is_array($timelinemap_settings_fields['table'])) {
		  uasort($timelinemap_settings_fields['table'], '_timelinemap_element_sort');
		  foreach($timelinemap_settings_fields['table'] as $key =>$value){
		    if($value['added'] == 1)
			  $field_information[] = array(
			    'key' => $key,
			    'alias' => $this->view->field[$key]->field_alias,
			    'handler' => $this->view->field[$key]->definition['handler'],
			  );
		  }
        }
      }
      return $field_information; 
    }
    else {
      return FALSE;
    }
  }
    /**
   * The function returns the field that was defined by the views timelinemap settings.
   *
   * @return
   *   array with the following attributes set:
   *
   *   alias - The alias of the field
   *   handler - A string with the name of the handler
   *   date_format _ If it is an supported date information the expected date format
   */
  function getDefinedFieldUsageDate($type, $date = NULL) {
    if ($timelinemap_settings_fields = $this->view->style_options['fields']) {
      if (isset($timelinemap_settings_fields[$type])) {
        if (is_string($timelinemap_settings_fields[$type])) {
          $fieldname = $timelinemap_settings_fields[$type];
        }
      }
      if (isset($fieldname) && $fieldname != '') {
        $field_information = array(
          'alias' => $this->view->field[$fieldname]->field_alias,
          'handler' => $this->view->field[$fieldname]->definition['handler'],
        );
        // check how to handle data from supported date fields
	  if (preg_match("/\d{4}\-\d{2}\-\d{2}(T| )\d{2}:\d{2}:\d{2}/", $date, $matches)) {
                $field_information['date_format'] = 'datetime';
      } 
      else $field_information['date_format'] = 'timestamp'; 
      return $field_information; 
      }
    }
    else {
      return FALSE;
    }
  }
  /**
   * Get the url of an image used as an icon. Based on the settings it can be pushed through an ImageCache preset.
   * Support for cck imagefields and taxonomy image module
   */
  function getIconUrl($row) {
    $field = $this->getDefinedFieldUsage('icon');
    if (isset($row->$field['alias'])) {
      if ($field['handler'] == 'views_handler_field_taxonomy_image' && module_exists('taxonomy_image')) {
        $path = file_directory_path() . '/' . variable_get('taxonomy_image_path', 'category_pictures') .'/';
        $path = $path . taxonomy_image_get_object($row->$field['alias'])->path;
      }

      elseif ($field['handler'] == 'content_handler_field_multiple') {
        $path = db_fetch_object(db_query('SELECT filepath FROM {files} WHERE fid = %d', $row->$field['alias']))->filepath;
      }
    }

    if (isset($path)) {
      if (module_exists('imagecache') &&
        $imagecache_namespace = $this->view->style_options['fields']['advanced']['imagecache_icon']) {
        return base_path() . imagecache_create_path($imagecache_namespace, $path);
      }
      else {
        return base_path() . $path;
      }
    }
  }
}