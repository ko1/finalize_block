require 'finalize_block'

finalize_block String => -> str do
  # clear procedure by replacing with emtpy string.
  str.replace ''
end do
  $g = 'a' * 256
  p $g.size #=> 256
end

p $g #=> '' # cleared
