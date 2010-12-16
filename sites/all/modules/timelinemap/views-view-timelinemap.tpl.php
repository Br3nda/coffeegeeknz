<?php
// $Id: views-view-timelinemap.tpl.php,v 1.2 2010/07/14 14:55:16 vesnaradivojevic Exp $
/**
 * @file views-view-timelinemap.tpl.php
 * Default simple view template to display a list of rows.
 *
 */
 ?>
<div id="timelinemap-all-<?php print $div_id; ?>" class = "timelinemap-all">
	<div class = "timelinecontainer" style = "width: <?php print $options['timelinemap']['width'];?>; height: <?php print $options['timelinemap']['tlheight'];?>">
	  <div id = "timeline-<?php print $div_id; ?>" class = "timeline"></div>
	</div>
	<div class = "mapcontainer" style = "width: <?php print $options['timelinemap']['width'];?>; height: <?php print $options['timelinemap']['mheight'];?>">
	  <div id = "map-<?php print $div_id; ?>" class = "map"></div>
	</div>
</div>

<?php
  global $user;
  $tlheight = str_replace('em', '', $options['timelinemap']['tlheight']);
  if($tlheight == $options['timelinemap']['tlheight']) 
    $meassurement_tl = 'px'; 
  else 
    $meassurement_tl = 'em';
  $tlheight = str_replace('px', '', $tlheight);
  $mheight = str_replace('em', '', $options['timelinemap']['mheight']);
  if($mheight == $options['timelinemap']['mheight']) 
    $meassurement_m = 'px'; 
  else 
    $meassurement_m = 'em';    
  $mheight = str_replace('px', '', $mheight);
  $height = $tlheight + $mheight;  
  if (user_access('embed timeline map') && $options['display']['embed_link'] && ($meassurement_m == $meassurement_tl)): 
?>
    <div id="timelinemap-embed-wrapper-<?php print $div_id; ?>"  style="text-align:right;font-size:80%;">
    <a id="timelinemap-<?php print $div_id; ?>-embed" class="timelinemap-embed-link" overwrite="" rel="timelinemap-all-<?php print $div_id; ?>" attrwidth="<?php print $options['timelinemap']['width']; ?>" attrheight="<?php print $height . $meassurement_m; ?>"  href="javascript: void(0);">[embed]</a></div> 
<?php
  endif;
?>