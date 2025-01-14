#! /usr/local/bin/perl

# Unicode to JIS X 0208

$file = "zcat JIS0208.TXT.gz |";

open( FILE, $file ) || die( "cannot open $file" );

while( <FILE> ){
  chop;
  if( /^#/ ){
     next;
   }
  ( $sjis, $jis, $uni, $rest ) = split;

  $count++;
  $array{ $uni } = pack( "A6A7", $jis, ", X0208" );
}

close( FILE );

# JIS X 0212

$file = "zcat JIS0212.TXT.gz |";

open( FILE, $file ) || die( "cannot open $file" );

while( <FILE> ){
  chop;
  if( /^#/ ){
     next;
   }
  ( $jis, $uni, $rest ) = split;

  if( $array{ $uni } ne "" ){
    print STDERR "Warning: duplicate character, uni:$uni, 0212:$jis\n";
    next;
  }

  $count++;
  $array{ $uni } = pack( "A6A7", $jis, ", X0212" );
}

close( FILE );

# Output

print "private codes_cset_t mapJIS[ $count ] = {\n";

for $index ( sort keys( %array ) ){
  $code = $array{ $index };
  $count--;
  if( $count == 0 ){
    print "  { $index, $code }\n";
  } else {
    print "  { $index, $code },\n";
  }
}

print "};\n";
