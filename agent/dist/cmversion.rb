# CMVersion v 1.0 #
# Remote CMS Version Enumerator #
# Coded By Luis Santana #
# HackTalk Security #
# Shoutz to Shardy, Xires&Stacy, Coors_Lite_Dude, RabHigh, Rage, Pure_Hate, g3k_, Reverend Digital, Eph, MaXe #

require "rubygems"
require "mechanize"
require "rexml/document"
require "logger"

if ARGV.length == 0
	puts "Usage: ruby #{$0} url\nexample: ruby #{$0} http://joomla.org"
	exit 0
end

site = ARGV[0]


agent = Mechanize.new
agent.log = Logger.new "/tmp/cmversion.log"
agent.user_agent_alias = "Mac Safari"

# Let's get to the fun stuff
# Running Joomla?
begin
	page = agent.get("#{site}/language/en-GB/en-GB.xml")
	version = page.body.match("<version>([0-9]\.*[0-9]).*")
	puts ("Joomla,#{version[1]}")
	exit
	rescue Mechanize::ResponseCodeError
	rescue NoMethodError
end

# How About Wordpress?
begin
    page = agent.get("#{site}/readme.html")
    version = page.body.match("Version.([0-9]\.*[0-9].*)")
    puts ("Wordpress,#{version[1]}")
    exit
    rescue Mechanize::ResponseCodeError
	rescue NoMethodError
end

# phpBB?
begin
    page = agent.get("#{site}/styles/prosilver/imageset/imageset.cfg")
    version = page.body.match("version.=.([0-9]\.*[0-9].*)")
    puts ("phpBB,#{version[1]}")
    exit
    rescue Mechanize::ResponseCodeError
	rescue NoMethodError
end

# Drupal?
begin
    page = agent.get("#{site}/CHANGELOG.txt")
    version = page.body.match("Drupal.([0-9]\.*[0-9].*),")
    puts ("Drupal,#{version[1]}")
    exit
    rescue Mechanize::ResponseCodeError
	rescue NoMethodError
end

# SMF
begin
    page = agent.get("#{site}/readme.html")
    version = page.body.match("SMF.([0-9]\.*[0-9].*)[A-Z].*Guide")
    puts ("SMF,#{version[1]}")
	exit
    rescue Mechanize::ResponseCodeError
	rescue NoMethodError
end

#page.search("/html/body/div[2]/div[3]/div[2]/table/tbody/tr/td[5]/a").each do |sploit|
#	puts "#{sploit.inner_html}"
#end
