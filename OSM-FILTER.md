# MOT-Specific OSM Files

Pfaedle now supports using different OSM files for different modes of transport (MOTs). This is useful when:
- **Long-distance trains** need a large OSM extract (e.g., `europe-latest.osm.pbf`)
- **Local transit** (bus, tram) can use smaller country-specific extracts (e.g., `germany-latest.osm.pbf`)

This significantly reduces processing time for local transit while maintaining accuracy for international routes.

## Configuration

Add `osm_file:` to any MOT section in `pfaedle.cfg`:

```cfg
[rail]
# Use Europe OSM for international trains
osm_file: europe-latest.osm.pbf

# ... rest of rail config ...

[bus, tram, subway]
# Use Germany OSM for local transit
osm_file: germany-latest.osm.pbf

# ... rest of config ...

[coach]
# Coach routes may cross borders
osm_file: europe-latest.osm.pbf
```

# Preparing osm.pbf files

```bash
# Combined extract by polygon + tag filter (RECOMMENDED)
osmium extract \
  --polygon germany.poly \
  --strategy complete_ways \
  europe-latest.osm.pbf \
  -o - \
  | osmium tags-filter - \
    w/highway=motorway,trunk,primary,secondary,tertiary,residential,living_street,unclassified,motorway_link,trunk_link,primary_link,secondary_link,tertiary_link,residential_link,bus_guideway,road,pedestrian,service,track,footway \
    w/railway=rail,light_rail,tram,narrow_gauge,subway,funicular,stop,halt,station,tram_stop,subway_stop \
    w/route=rail,light_rail,train,bus,trolleybus,ferry,tram,subway,funicular \
    w/aerialway=gondola,cable_car,chair_lift,mixed_lift,station,stop \
    w/waterway=river \
    w/public_transport \
    w/way=primary,secondary,bus_guideway \
    w/busway \
    w/psv=yes,designated \
    w/bus=yes,designated \
    w/minibus=yes,designated \
    w/trolley_wire=yes \
    w/trolleywire=yes \
    w/trolleybus=yes \
    w/trolley_bus=yes \
    w/motorboat=yes \
    w/ferry=yes \
    w/subway=yes \
    w/tram=yes \
    w/lanes:bus \
    w/lanes:psv \
    w/bus:lanes \
    n/public_transport=stop_position \
    n/railway=stop,halt,station,tram_stop,subway_stop \
    n/bus_stop \
    n/highway=bus_stop,turning_circle,turning_loop \
    n/amenity=bus_station,ferry_terminal \
    n/aerialway=station,stop \
    n/station=subway,tram,ferry \
    n/mooring=ferry \
    n/tram_stop \
    n/stop \
    r/type=route,route_master,restriction,restriction:bus,multipolygon \
    r/route=rail,light_rail,train,bus,trolleybus,ferry,tram,subway,funicular \
    r/public_transport=stop_area \
    -o germany-transit.osm.pbf \
    --overwrite

# Alternative: Use bbox instead of .poly file
osmium extract \
  --bbox 5.8,47.2,15.1,55.1 \
  --strategy complete_ways \
  europe-latest.osm.pbf \
  -o - \
  | osmium tags-filter - \
    w/highway \
    w/railway \
    w/route \
    w/aerialway \
    w/waterway=river \
    w/public_transport \
    w/busway \
    w/psv \
    w/bus \
    w/trolley_wire=yes \
    w/ferry=yes \
    nwr/type=route,route_master,restriction \
    -o germany-transit-simple.osm.pbf \
    --overwrite
```