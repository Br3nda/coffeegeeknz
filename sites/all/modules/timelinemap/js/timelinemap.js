// $Id: timelinemap.js,v 1.2 2010/07/14 14:55:16 vesnaradivojevic Exp $

/*
 *  @file timeline.js
 */	

Drupal.timelinemap = {
  addBonusLayers: function(mapid, map) {
    var mapTypes = {
				'map' : G_NORMAL_MAP,
				'satellite' : G_SATELLITE_MAP,
				'hybrid' : G_HYBRID_MAP,
				'physical' : G_PHYSICAL_MAP
	};  
	var default_set = false;
	var layers = Drupal.settings.timelinemap[mapid].layer;
	var defaultlayer = Drupal.settings.timelinemap[mapid].defaultlayer;
	if(layers != undefined && layers != null){
	  	for ( var i = 0; i <layers.length; i++) {
	  	    var custommap = GMapBox(layers[i].name, layers[i].title, layers[i].options);
	  	    map.addMapType(custommap);
	  	    if(defaultlayer == layers[i].name && !default_set){
	  	      map.setMapType(GMapBox(layers[i].name, layers[i].title, layers[i].options));
	  	      default_set = true;
	  	    }
	  	    
	  	}
	}
	var overlays = Drupal.settings.timelinemap[mapid].overlay;
	if(overlays != undefined && overlays != null){
	  	for ( var i = 0; i <overlays.length; i++) {
	  	    var customoverlay  = GMapBox(overlays[i].name, overlays[i].title, overlays[i].options);
	  	    map.addOverlay(customoverlay);
	  	}
	}
	if (!default_set)
	    map.setMapType(mapTypes[defaultlayer]); 
  },
  
  
  createWidget: function(args) {
	items = [];
	$.each(args.events, function(j, u){ 
		if(u.color == 'light blue' || u.color == 'Light blue')
			var color = 'ltblue';
		else var color = u.color;
		
		if(u.link)
		    var htmltitle = "<a href = '" + u.link + "'>" + u.title + "</a>";
		else 
            var htmltitle = u.title; 
		var description = "";	
		$.each(u.descriptiontext_new, function(ind, val){
		    description += '<div class = "desc-item-'+ind+'">'+val+'</div>';
		});
		items[j] = {"start" : u.start,
					
					 "point" :  {
							  "lat" : u.latitude,
							  "lon" : u.longitude
						   },
					 "title" : u.title,
					 "options" : {
							// use the default title/description info window
							//"infoHtml": htmltitle + '<div class = "description">' + ((u.icon != null) ? '<div class = "image"><img src = "' + u.icon + '" /></div>' : '') + ((description != null) ? description : '') + '</div>',
							"infoHtml": '<div class = "title">' + htmltitle + '</div>' + '<div class = "description">'+ ((u.icon != null) ? '<div class = "image"><img src = "' + u.icon + '" /></div>' : '') + ((description != null) ? description : '') + '</div>',
							"theme": color
					 }
					};
		if(u.end)				
			items[j]["end"] = u.end;
	}); 
	var mainBandInt;
	var summBandInt;
	switch (args.band1_unit) {
		case 'SECOND': mainBandInt = Timeline.DateTime.SECOND; break;
		case 'MINUTE': mainBandInt = Timeline.DateTime.MINUTE; break;
		case 'HOUR': mainBandInt = Timeline.DateTime.HOUR; break;
		case 'DAY': mainBandInt = Timeline.DateTime.DAY; break;
		case 'WEEK': mainBandInt = Timeline.DateTime.WEEK; break;
		case 'MONTH': mainBandInt = Timeline.DateTime.MONTH; break;
		case 'YEAR': mainBandInt = Timeline.DateTime.YEAR; break;
		case 'DECADE': mainBandInt = Timeline.DateTime.DECADE; break;
		case 'CENTURY': mainBandInt = Timeline.DateTime.CENTURY; break;
		case 'MILLENNIUM': mainBandInt = Timeline.DateTime.MILLENNIUM; break;
		default: mainBandInt = Timeline.DateTime.YEAR;
	}
	switch (args.band2_unit) {
		case 'SECOND': summBandInt = Timeline.DateTime.SECOND; break;
		case 'MINUTE': summBandInt = Timeline.DateTime.MINUTE; break;
		case 'HOUR': summBandInt = Timeline.DateTime.HOUR; break;
		case 'DAY': summBandInt = Timeline.DateTime.DAY; break;
		case 'WEEK': summBandInt = Timeline.DateTime.WEEK; break;
		case 'MONTH': summBandInt = Timeline.DateTime.MONTH; break;
		case 'YEAR': summBandInt = Timeline.DateTime.YEAR; break;
		case 'DECADE': summBandInt = Timeline.DateTime.DECADE; break;
		case 'CENTURY': summBandInt = Timeline.DateTime.CENTURY; break;
		case 'MILLENNIUM': summBandInt = Timeline.DateTime.MILLENNIUM; break;
		default: summBandInt = Timeline.DateTime.DECADE;
	}	
    
	//if pathline type: pathcolor
	var pathcolor = args.pathcolor;
	if(args.datasource == 'basic'){
		tm = TimeMap.init({
			mapId: "map-"+args.display_name,               // Id of map div element (required)
			timelineId: "timeline-"+args.display_name,     // Id of timeline div element (required)
			options: {
				eventIconPath: Drupal.settings.basePath + Drupal.settings.timelinemap_path +'/images/'
			},
			datasets: [
				{
					id: "timelinemap",
					title: "Timelinemap",
					theme: "red",
					type: "basic",
					options: {
						items: items
					}
				}
			],
			bandIntervals: [
				mainBandInt, 
				summBandInt
			]
		});
		
		Drupal.timelinemap.addBonusLayers(args.display_name, tm.map);
		
	} else if(args.datasource == 'pathline'){
	    tm = TimeMap.init({
			mapId: "map-"+args.display_name,               // Id of map div element (required)
			timelineId: "timeline-"+args.display_name,     // Id of timeline div element (required)
			options: {
				eventIconPath: Drupal.settings.basePath + Drupal.settings.timelinemap_path +'/images/'
			},
			datasets: [
				{
					id: "timelinemap",
					title: "Timelinemap",
					theme: "red",
					type: "basic",
					options: {
						items: items
					}
				}
			],
			bandIntervals: [
				mainBandInt, 
				summBandInt
			],
			dataDisplayedFunction: function(tm) {
				// new filter chain for map lines
				tm.addFilterChain("pathlines",
					// true condition: add a pathline
					function(item) {
						if (!item.pathline && !item.skip) {
							item.pathline = new GPolyline([item.opts.infoPoint, item.next.opts.infoPoint], pathcolor);
							tm.map.addOverlay(item.pathline);
						}
					},
					// false condition: remove a pathline
					function(item) {
						if (item.pathline) {
							tm.map.removeOverlay(item.pathline);
							item.pathline = null;
						}
					}
				);
				// define the filter that creates the pathlines
				tm.addFilter("pathlines", function(item) {
					if (!item.next && item.event && !item.skip) {
						// use the Timeline sequential iterator
						var i = tm.timeline.getBand(0).getEventSource().getEventIterator(item.event.getStart(), new Date());
						FINDNEXT: {
							while (!item.next) {
								if (i.hasNext()) {
									var next = i.next().item;
									// if the next one's taken we can skip
									if (next.taken) {
										item.skip = true;
										break FINDNEXT;
									}
									// skip missing placemarks at the same location
									if (next.placemark && !item.opts.infoPoint.equals(next.opts.infoPoint)) {
										item.next = next;
										next.taken = true;
									}
								}
								else break FINDNEXT;
							}
						}
					}
					return (item.next && item.placemarkVisible && item.next.placemarkVisible);
				});
				// invoke the filter once data is displayed
				tm.filter("pathlines");
				// update map on timeline scroll
				tm.timeline.getBand(0).addOnScrollListener(function() {
					tm.filter("pathlines");
				});
			}
		});
		
		Drupal.timelinemap.addBonusLayers(args.display_name, tm.map);
	}
  },
  
	embed : function(args, id, rel, attrwidth, attrheight, overwrite) {
		if (GBrowserIsCompatible()) {
			var divIdArr = rel.split('-');
			var divId = divIdArr.pop();
			var divId = divIdArr.pop() + "-" + divId;
			var timelinecontainerStyle = $('#'+rel).children('.timelinecontainer').attr('style');
			var mapcontainer = $('#'+rel).children('.mapcontainer').attr('style');
			var scripts = document.getElementsByTagName('script');
			var scrString = "";
			for ( var i = 0; i < scripts.length; i++) {
				var tmp = scripts.item(i).text;
				if (tmp.search('datasource') != -1 && tmp.search('timelinemap') != -1)
					scrString = tmp;
	
			}
			var path = Drupal.settings.basePath + 'timelinemap/embed';
			$.ajax( {
				type : "POST",
				url : path,
				data : "id=" + id + "&rel=" + rel
						+ "&settings=" + scrString+"&timelinecontainerStyle="+timelinecontainerStyle
						+"&mapcontainer="+mapcontainer+"&divId="+divId+"&attrwidth="+attrwidth+"&attrheight="+attrheight+"&storage="+args.storage
						+"&vidiLink="+args.vidi_link+"&viewLink="+args.view_link+"&overwrite="+overwrite+"&datasource="+args.datasource+"&title="+args.visualization_title,
				success : function(msg) {
					var divid = $('#' + id).parent().attr('id');
					$('#' + divid).hide('fast');
					$('#' + divid).html(msg);
					$('#' + divid).fadeIn('slow');
				},
				error : function(msg) {
					alert('error');
				}
			});
		}
	}  


}; 
Drupal.behaviors.timelinemap = function(context) {
  if(Drupal.settings.timelinemap) { 
	 $('.timelinemap-all').each(function() { var d=0;
	        var currentId = $(this).attr('id');
			$.each(Drupal.settings.timelinemap, function(i, v) {
			  if(currentId == 'timelinemap-all-'+i)
			  Drupal.timelinemap.createWidget(v); 
			});
			
	 })
	$('.timelinemap-embed-link', context).click(function() {
	    var rel = $(this).attr('rel');
		var id = $(this).attr('id');
		var attrwidth = $(this).attr('attrwidth');
		var attrheight = $(this).attr('attrheight');
		var overwrite = $(this).attr('overwrite');
		var title = $(this).attr('title');
		$.each(Drupal.settings.timelinemap, function(i, v) {
			  Drupal.timelinemap.embed(v, id, rel, attrwidth, attrheight, overwrite); 
			});
	});	 
  }
}