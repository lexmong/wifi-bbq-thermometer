var app = angular.module('app', ['ngRoute']).run(function ($rootScope) {
    //default unit
    $rootScope.unit = 'c';
    $rootScope.n_probes = 2;
});

app.config(function ($routeProvider, $locationProvider) {
    $routeProvider.when('/', {
        controller: 'tempCtrl',
        templateUrl: '/probes.html',
    });
});

app.config(function ($routeProvider, $locationProvider) {
    $routeProvider.when('/config', {
        controller: 'configCtrl',
        templateUrl: '/config.html',
    });
});

/**
 * Probe controller
 */
app.controller('tempCtrl', ['$scope','$rootScope', '$http','$interval', 
    function ($scope, $rootScope,$http, $interval) {

    $scope.probes = [];
    for(let i=1;i<=$rootScope.n_probes;i++){
      $scope.probes.push({
        id:i,
        target:'-',
        temperature:-1
      });
    }

    function getTemp() {
        $http.get('/readTemperature',{
            params: {unit: $rootScope.unit}
        }).then((response) => {
            for(let i =0;i<$scope.probes.length;i++){
                $scope.probes[i].temperature = Math.round(response.data[i]);
            }
        }, (response) => {
            console.error(response);
            for(let i =0;i<$scope.probes.length;i++){
                $scope.probes[i].temperature =-1;
            }
        });
    }

    getTemp();

    update = $interval(function() {
        getTemp();
    }, 2000);

    $scope.$on('$destroy', function() {
        $interval.cancel(update);
    });
}]);

/**
 * Configuration controller
 */ 
app.controller('configCtrl', ['$scope','$rootScope','$http', 
    function ($scope, $rootScope,$http) {
    
    //Read probe resistance
    $scope.readResistance = (i) => {
        $http.get('/readResistance',{
            params: {probe: $scope.probe}
        }).then((response) => {
            console.log(response);
            $scope.points[i].resistance = response.data[0];
        }, (response) => {
            console.error(response);
        });
    };

    //On probe select change
    $scope.changeProbe = () => {
        $scope.points = [
            {temperature:null,resistance:null},
            {temperature:null,resistance:null},
            {temperature:null,resistance:null},
        ];

        $scope.coeffs = [];
    };

    $scope.submitPoints = () => {
        $http.get('/saveCoeffs',{
            params: {
                points: JSON.stringify($scope.points),
                probe: $scope.probe,
                unit: $rootScope.unit
            }
        }).then((response) => {
            console.log(response);
            $scope.coeffs = [
                response.data.a,
                response.data.b,
                response.data.c
            ];
        }, (response) => {
            console.error(response);
        });
    };

    $scope.toAlpha = (i) => String.fromCharCode(i+65);
    $scope.changeUnit = (unit)=>{$rootScope.unit = unit};

    $scope.changeProbe();
}]);

/**
 * Navbar controller 
 */
app.controller('navCtrl',['$scope','$location',function ($scope,$location){
    $scope.links = [
        {text:'Probes',href:'/'},
        {text:'Configuration',href:'/config'},
    ]; 

    //set navbar option class
    $scope.class = (page) => page == $location.path() ? 'active':'';
}]);
