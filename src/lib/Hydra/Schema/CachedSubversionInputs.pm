package Hydra::Schema::CachedSubversionInputs;

use strict;
use warnings;

use base 'DBIx::Class';

__PACKAGE__->load_components("Core");
__PACKAGE__->table("CachedSubversionInputs");
__PACKAGE__->add_columns(
  "uri",
  { data_type => "text", is_nullable => 0, size => undef },
  "revision",
  { data_type => "integer", is_nullable => 0, size => undef },
  "sha256hash",
  { data_type => "text", is_nullable => 0, size => undef },
  "storepath",
  { data_type => "text", is_nullable => 0, size => undef },
);
__PACKAGE__->set_primary_key("uri", "revision");


# Created by DBIx::Class::Schema::Loader v0.04005 @ 2009-03-06 14:20:12
# DO NOT MODIFY THIS OR ANYTHING ABOVE! md5sum:O0QvXjOulMVTjhW4rRHkmQ


# You can replace this text with custom content, and it will be preserved on regeneration
1;
