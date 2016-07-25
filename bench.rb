require 'pp'
require 'json'

result = {}
(1..10).to_a.each{|n|
  size = n * 10000
  ns = `./hashbench #{size}`.chomp.to_i
  result[size] = ns / size
  puts "#{size} -> #{ns / size}"
}

File.open("extending.json", "w") {|f| f.write(result.to_json) }
