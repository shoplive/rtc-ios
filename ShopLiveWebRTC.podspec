Pod::Spec.new do |spec|
  spec.name         = "ShopLiveWebRTC"
  spec.version      = "1.0.14"
  spec.summary      = "ShopLive SDK for iOS"

  spec.homepage     = "http://shoplive.cloud"
  spec.source   = { :git => 'https://github.com/shoplive/rtc-ios.git', :tag => spec.version }
  spec.license = { :type => 'Copyright', :text => <<-LICENSE
                 Copyright 2021
                 Permission is granted to...
                 LICENSE
              }

  spec.author             = { "Shoplive" => "shoplive-eng@shoplive.cloud" }
  spec.platform     = :ios
  spec.ios.deployment_target = '13.0'
  spec.swift_version = "5"
  spec.vendored_frameworks = 'Frameworks/WebRTC.xcframework'
end
